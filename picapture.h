/**
 * picapture.h
 *
 * Basic ASI Stuff
 **/

#ifndef __PICAPTURE__H__
#define __PICAPTURE__H__

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

/**
 * 
 */

#define ASI_CALL(function)						\
	do {								\
		asi_error_code = (function);				\
		if (asi_error_code != ASI_SUCCESS) {			\
			fprintf(stderr,					\
				"%s:%d - ASI Error: %d [%s]\n",		\
				__FILE__, __LINE__, asi_error_code,	\
				asi_error_code_string(asi_error_code)); \
			goto error;					\
		}							\
	} while (0)

#endif	/* __PICAPTURE__H__ */
