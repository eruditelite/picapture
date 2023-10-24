/**
 * info.c
 *
 * A simple example in C to build/use the ZWO ASI SDK.
 **/

#include <stdio.h>
#include <stdlib.h>

#include <ASICamera2.h>

static const char *asi_error_code_string(ASI_ERROR_CODE error)
{
	switch (error) {
		/* operation was successful */
	case ASI_SUCCESS: return "ASI_SUCCESS";
		/*no camera connected or index value out of boundary */
	case ASI_ERROR_INVALID_INDEX: return "ASE_ERROR_INVALID_INDEX";
		/*invalid ID */
	case ASI_ERROR_INVALID_ID: return "ASI_ERROR_INVALID_ID";
		/*invalid control type */
	case ASI_ERROR_INVALID_CONTROL_TYPE:
		return "ASI_ERROR_INVALID_CONTROL_TYPE";
		/*camera didn't open */
	case ASI_ERROR_CAMERA_CLOSED: return "ASI_ERROR_CAMERA_CLOSED";
		/*failed to find the camera, maybe the camera has been removed */
	case ASI_ERROR_CAMERA_REMOVED: return "ASI_ERROR_CAMERA_REMOVED";
		/*cannot find the path of the file */
	case ASI_ERROR_INVALID_PATH: return "ASI_ERROR_INVALID_PATH";
	case ASI_ERROR_INVALID_FILEFORMAT: return "ASI_ERROR_INVALID_FILEFORMAT";
		/*wrong video format size */
	case ASI_ERROR_INVALID_SIZE: return "ASI_ERROR_INVALID_SIZE";
		/*unsupported image format */
	case ASI_ERROR_INVALID_IMGTYPE: return "ASI_ERROR_INVALID_IMGTYPE";
		/*the startpos is outside the image boundary */
	case ASI_ERROR_OUTOF_BOUNDARY: return "ASI_ERROR_OUTOF_BOUNDARY";
		/*timeout */
	case ASI_ERROR_TIMEOUT: return "ASI_ERROR_TIMEOUT";
		/*stop capture first */
	case ASI_ERROR_INVALID_SEQUENCE: return "ASI_ERROR_INVALID_SEQUENCE";
		/*buffer size is not big enough */
	case ASI_ERROR_BUFFER_TOO_SMALL: return "ASI_ERROR_BUFFER_TOO_SMALL";
	case ASI_ERROR_VIDEO_MODE_ACTIVE: return "ASI_ERROR_VIDEO_MODE_ACTIVE";
	case ASI_ERROR_EXPOSURE_IN_PROGRESS:
		return "ASI_ERROR_EXPOSURE_IN_PROGRESS";
		/*general error, eg: value is out of valid range */
	case ASI_ERROR_GENERAL_ERROR: return "ASI_ERROR_GENERAL_ERROR";
	case ASI_ERROR_END: return "ASI_ERROR_END";
	default:
		fprintf(stderr, "Unknown ASI_ERROR_CODE! [%d]\n", error);
		break;
	}

	return "Unknown ASI_ERROR_CODE";
}

static const char *asi_control_type_string(ASI_CONTROL_TYPE control)
{
	switch (control) {
	case ASI_GAIN: return "ASI_GAIN";
	case ASI_EXPOSURE: return "ASI_EXPOSURE";
	case ASI_GAMMA: return "ASI_GAMMA";
	case ASI_WB_R: return "ASI_WB_R";
	case ASI_WB_B: return "ASI_WB_B";
	case ASI_OFFSET: return "ASI_OFFSET";
	case ASI_BANDWIDTHOVERLOAD: return "ASI_BANDWIDTHOVERLOAD";
	case ASI_OVERCLOCK: return "ASI_OVERCLOCK";
	case ASI_TEMPERATURE: return "ASI_TEMPERATURE";
	case ASI_FLIP: return "ASI_FLIP";
	case ASI_AUTO_MAX_GAIN: return "ASI_AUTO_MAX_GAIN";
	case ASI_AUTO_MAX_EXP: return "ASI_AUTO_MAX_EXP";
	case ASI_AUTO_TARGET_BRIGHTNESS: return "ASI_AUTO_TARGET_BRIGHTNESS";
	case ASI_HARDWARE_BIN: return "ASI_HARDWARE_BIN";
	case ASI_HIGH_SPEED_MODE: return "ASI_HIGH_SPEED_MODE";
	case ASI_COOLER_POWER_PERC: return "ASI_COOLER_POWER_PERC";
	case ASI_TARGET_TEMP: return "ASI_TARGET_TEMP";
	case ASI_COOLER_ON: return "ASI_COOLER_ON";
	case ASI_MONO_BIN: return "ASI_MONO_BIN";
	case ASI_FAN_ON: return "ASI_FAN_ON";
	case ASI_PATTERN_ADJUST: return "ASI_PATTERN_ADJUST";
	case ASI_ANTI_DEW_HEATER: return "ASI_ANTI_DEW_HEATER";
	case ASI_FAN_ADJUST: return "ASI_FAN_ADJUST";
	case ASI_PWRLED_BRIGNT: return "ASI_PWRLED_BRIGNT";
	case ASI_GPS_SUPPORT: return "ASI_GPS_SUPPORT";
	case ASI_GPS_START_LINE: return "ASI_GPS_START_LINE";
	case ASI_GPS_END_LINE: return "ASI_GPS_END_LINE";
	case ASI_ROLLING_INTERVAL: return "ASI_ROLLING_INTERVAL";
	default:
		fprintf(stderr, "Unknown ASI_CONTROL_TYPE! [%d]\n", control);
		break;
	}

	return "Unknown ASI_CONTROL_TYPE";
}

static const char *asi_img_type_string(ASI_IMG_TYPE type)
{
	switch (type) {
	case ASI_IMG_RAW8: return "ASI_IMG_RAW8";
	case ASI_IMG_RGB24: return "ASI_IMG_RGB24";
	case ASI_IMG_RAW16: return "ASI_IMG_RAW16";
	case ASI_IMG_Y8: return "ASI_IMG_Y8";
	default:
		fprintf(stderr, "Unknown ASI_IMG_TYPE! [%d]\n", type);
		break;
	}

	return "Unknown ASI_IMG_TYPE";
}

static void display_camera_info(ASI_CAMERA_INFO *info)
{
	int i;

	printf("-- Camera Information for %s [%d] --\n"
	       "   Height: %ld Width: %ld Pixel Size: %f Bit Depth: %d\n"
	       "   Color? %s\n"
	       "   Mechanical Shutter? %s\n"
	       "   Cooler? %s\n",
	       info->Name, info->CameraID,
	       info->MaxHeight, info->MaxWidth, info->PixelSize, info->BitDepth,
	       info->IsColorCam ? "Yes" : "No",
	       info->MechanicalShutter ? "Yes" : "No",
	       info->IsCoolerCam ? "Yes" : "No");

	printf("   Supported Bins: ");

	for (i = 0; i < 16; ++i) {
		int bin_supported;

		bin_supported = info->SupportedBins[i];

		if (!bin_supported)
			break;

		printf("Bin%d ", i + 1);
	}

	printf("\n");

	printf("   Supported Video Modes: ");

	for (i = 0; i < 8; ++i) {
		ASI_IMG_TYPE image_type;

		image_type = info->SupportedVideoFormat[i];

		if (image_type == ASI_IMG_END)
			break;

		printf("%s ", asi_img_type_string(image_type));
	}

	printf("\n");
}

#define ASI_CALL(function)						\
	do {								\
		ASI_ERROR_CODE ec = (function);				\
		if (ec != ASI_SUCCESS) {				\
			fprintf(stderr,					\
				"%s:%d - ASI Error: %d\n",		\
				__FILE__, __LINE__, ec);		\
			goto error;					\
		}							\
	} while (0)

int main(void)
{
	int i, num_cameras;
	ASI_ERROR_CODE asi_error_code;

	num_cameras = ASIGetNumOfConnectedCameras();

	if (!num_cameras) {
		fprintf(stderr, "No cameras detected!\n");
		return EXIT_FAILURE;
	}

	if (num_cameras == 1)
		printf("1 camera detected!\n");
	else
		printf("%d cameras detected!\n", num_cameras);

	for (i = 0; i < num_cameras; ++i) {
		ASI_CAMERA_INFO info;
		int j, num_controls;

		ASI_CALL(ASIGetCameraProperty(&info, i));
		display_camera_info(&info);

		ASI_CALL(ASIOpenCamera(i));
		ASI_CALL(ASIInitCamera(i));
		ASI_CALL(ASIGetNumOfControls(i, &num_controls));
		printf("-- Capabilities --\n");

		for (j = 0; j < num_controls; ++j) {
			ASI_CONTROL_CAPS caps;
			long value;
			ASI_BOOL isauto;

			ASI_CALL(ASIGetControlCaps(i, j, &caps));
			ASI_CALL(ASIGetControlValue(i, caps.ControlType,
						    &value, &isauto));
			printf("%s [%s/%d] - %s\n",
			       caps.Name,
			       asi_control_type_string(caps.ControlType),
			       caps.ControlType, caps.Description);
			printf("    Auto Supported? %d Writeable? %d Auto On? %d",
			       caps.IsAutoSupported, caps.IsWritable, isauto);
			printf(" Current: %ld Default: %ld Min: %ld Max: %ld\n",
			       value, caps.DefaultValue,
			       caps.MinValue, caps.MaxValue);
		}

		ASI_CALL(ASICloseCamera(i));
	}

	return EXIT_SUCCESS;

 error:

	return EXIT_FAILURE;
}
