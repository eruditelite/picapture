/**
 * picapture.c
 *
 * A simple example in C to build/use the ZWO ASI SDK.
 **/

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "picapture.h"

/**
 * The 'info' option.
 */

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

static int info(int camera)
{
	int i, num_cameras, num_controls;
	ASI_ERROR_CODE asi_error_code;
	ASI_CAMERA_INFO info;

	/* ASIGetNumOfConnectedCameras() is always required! */
	num_cameras = ASIGetNumOfConnectedCameras();

	if (!num_cameras) {
		fprintf(stderr, "No cameras detected!\n");
		return EXIT_FAILURE;
	}

	if (num_cameras < camera + 1) {
		fprintf(stderr, "Camera %d doesn't exist!\n", camera);
		return EXIT_FAILURE;
	}

	ASI_CALL(ASIGetCameraProperty(&info, camera));
	display_camera_info(&info);

	ASI_CALL(ASIOpenCamera(camera));
	ASI_CALL(ASIInitCamera(camera));
	ASI_CALL(ASIGetNumOfControls(camera, &num_controls));
	printf("-- Capabilities --\n");

	for (i = 0; i < num_controls; ++i) {
		ASI_CONTROL_CAPS caps;
		long value;
		ASI_BOOL isauto;

		ASI_CALL(ASIGetControlCaps(camera, i, &caps));
		ASI_CALL(ASIGetControlValue(camera, caps.ControlType,
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

	ASI_CALL(ASICloseCamera(camera));

	return EXIT_SUCCESS;

 error:

	return EXIT_FAILURE;
}

/**
 * The 'list' option.
 */

static int list(void)
{
	ASI_ERROR_CODE asi_error_code;
	ASI_CAMERA_INFO info;
	int i, num_cameras;

	num_cameras = ASIGetNumOfConnectedCameras();

	if (!num_cameras) {
		fprintf(stderr, "No cameras detected!\n");
		return EXIT_FAILURE;
	}

	for (i = 0; i < num_cameras; ++i) {
		ASI_CALL(ASIGetCameraProperty(&info, i));
		printf("-- %s [index %d] --\n", info.Name, info.CameraID);
	}

	return EXIT_SUCCESS;

 error:

	return EXIT_FAILURE;
}

/**
 * main()
 *
 * Get the agruments, set up the run and do it...
 */

/* Help */

static void help(int exit_code)
{
	printf("picapture: [--help|-h] [--info|-i camera] [--list|-l]\n"
	       "\t--help|-h : Display this wonderful help screen!\n"
	       "\t--info|-i : Print details of the specified camera.\n"
	       "\t--list|-l : List available cameras.\n");

	exit(exit_code);
}

/* Arguments */

static const char *sopts = "hi:l";
static const struct option lopts[] = {
	{"help", no_argument,       NULL, 'h'},
	{"info", required_argument, NULL, 'i'},
	{"list", no_argument,       NULL, 'l'}
};

int main(int argc, char *argv[])
{
	int i, num_cameras, opt;
	ASI_ERROR_CODE asi_error_code;
	char *command = NULL;

	/* Process Arguments */
	while ((opt = getopt_long(argc, argv, sopts, lopts, NULL)) != EOF) {
		switch (opt) {
		case 'h':
			help(0);
			break;
		case 'i':
			info(strtol(optarg, NULL, 0));
			break;
		case 'l':
			list();
			break;
		default:
			break;
		}
	}

	return EXIT_SUCCESS;
}
