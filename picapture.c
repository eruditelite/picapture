/**
 * picapture.c
 *
 * A simple example in C to build/use the ZWO ASI SDK.
 **/

#include <stdio.h>
#include <stdlib.h>

#include "picapture.h"

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