/**
 * picapture.c
 *
 * A simple example in C to build/use the ZWO ASI SDK.
 **/

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "picapture.h"

/**
 * The 'capture' option.
 */

static int process_control(int camera, char *option)
{
	printf("%s:%d - camera=%d option=%s\n",
	       __FILE__, __LINE__, camera, option);

	return EXIT_SUCCESS;
}

static int save_rgb24_to_ppm(int width, int height,
			     void *data, const char *file)
{
	int i, j;
	FILE *output;
	unsigned char *buffer;

	buffer = data;

	output = fopen(file, "wb");
	if (!output) {
		fprintf(stderr, "fopen() failed: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	fprintf(output, "P6\n%d %d\n255\n", width, height);

	for (i = 0; i < height; ++i) {
		for (j = 0; j < width; ++j) {
			unsigned char color[3];
			color[0] = *buffer++;
			color[1] = *buffer++;
			color[2] = *buffer++;
			fwrite(color, 1, 3, output);
		}
	}

	fclose(output);

	return EXIT_SUCCESS;
}

static int capture(char *options)
{
	int camera = -1, i, num_cameras, number, rc = 0;
	ASI_ERROR_CODE asi_error_code;
	char *option, *prefix;
	ASI_CAMERA_INFO info;
	long exposure;

	option = strtok(options, ",");
	if (!option)
		fprintf(stderr, "Bad Options: %s\n", options);
	camera = strtol(option, NULL, 0);
	option = strtok(NULL, ",");
	if (!option)
		fprintf(stderr, "Bad Options: %s\n", options);
	prefix = strndup(option, FILENAME_MAX - 5);
	option = strtok(NULL, ",");
	if (!option)
		fprintf(stderr, "Bad Options: %s\n", options);
	number = strtol(option, NULL, 0);
	option = strtok(NULL, ",");
	if (!option)
		fprintf(stderr, "Bad Options: %s\n", options);
	exposure = strtol(option, NULL, 0);

	/* ASIGetNumOfConnectedCameras() is always required! */
	num_cameras = ASIGetNumOfConnectedCameras();

	if (!num_cameras) {
		fprintf(stderr, "No cameras detected!\n");
		return EXIT_FAILURE;
	}

	if (camera == -1) {
		fprintf(stderr, "Camera must be set!\n");
		return EXIT_FAILURE;
	}

	if (num_cameras < camera + 1) {
		fprintf(stderr, "Camera %d doesn't exist!\n", camera);
		return EXIT_FAILURE;
	}

	ASI_CALL(ASIGetCameraProperty(&info, camera));
	ASI_CALL(ASIOpenCamera(camera));
	ASI_CALL(ASIInitCamera(camera));

	option = strtok(NULL, ",");

	while(option) {
		rc = process_control(camera, option);

		if (rc) {
			fprintf(stderr, "%s:%d - process_control() failed: %d\n",
				__FILE__, __LINE__, rc);
			return EXIT_FAILURE;
		}

		option = strtok(NULL, ",");
	}

	for (i = 0; i < number; ++i) {
		char output[FILENAME_MAX];
		ASI_EXPOSURE_STATUS status;
		void *buffer;
		size_t buffer_size;
		ASI_IMG_TYPE type;

		/**
		 * From the ZWO SDK Documentation
		 *
		 * -- For RAW8 and Y8 --
		 * bufSize >= image_width*image_height
		 * -- For RAW16 --
		 * bufSize >= image_width*image_height * 2
		 * -- For RGB24 --
		 * bufSiz >= image_width*image_height * 3
		 *
		 * iWaitms value: exposure_time * 2 + 500ms
		 */

		type = ASI_IMG_RGB24;
		ASI_CALL(ASISetROIFormat(camera,
					 info.MaxWidth, info.MaxHeight,
					 1, type));
		ASI_CALL(ASISetControlValue(camera, ASI_EXPOSURE,
					    exposure, ASI_TRUE));

		buffer_size = info.MaxWidth * info.MaxHeight;

		switch (type) {
		case ASI_IMG_RAW8:
		case ASI_IMG_Y8:
			break;
		case ASI_IMG_RAW16:
			buffer_size *= 2;
			break;
		case ASI_IMG_RGB24:
			buffer_size *= 3;
			break;
		default:
			fprintf(stderr, "Unknown Image Type!\n");
			return EXIT_FAILURE;
		}

		printf("%s:%d - Capturing %d image(s) [%s] as %sNNNNN.ppm\n",
		       __FILE__, __LINE__,
		       number, asi_img_type_string(type), prefix);

		buffer = malloc(buffer_size);

		for (i = 0; i < number; ++i) {
			sprintf(output, "%s%05d.ppm", prefix, i);
			printf("%s:%d - Capturing to %s\n",
			       __FILE__, __LINE__, output);
			ASI_CALL(ASIStartExposure(camera, ASI_FALSE));

			while (1) {
				ASI_CALL(ASIGetExpStatus(camera, &status));

				if (status == ASI_EXP_SUCCESS)
					break;
			}

			ASI_CALL(ASIStopExposure(camera));
			ASI_CALL(ASIGetDataAfterExp(camera,
						    buffer, buffer_size));

			printf("%s:%d - Creating %s, %ldx%ld",
			       __FILE__, __LINE__,
			       output, info.MaxWidth, info.MaxHeight);
			save_rgb24_to_ppm(info.MaxWidth, info.MaxHeight,
					  buffer, output);
			printf("\n");
		}

		free(buffer);
	}

	ASI_CALL(ASICloseCamera(camera));

	return EXIT_SUCCESS;

 error:

	return EXIT_FAILURE;
}

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
		printf("%s [index %d]\n", info.Name, info.CameraID);
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
	printf("picapture: [--capture|-c options] [--help|-h] "
	       "[--info|-i camera] [--list|-l]\n"
	       "\t--capture|-c : Capture!\n"
	       "\t\tComma Separated String of Options\n"
	       "\t\t<camera>,<prefix>,<number>,<name>,<exposure>,<auto | value>,...\n"
	       "\t--help|-h    : Display this wonderful help screen!\n"
	       "\t--info|-i    : Print details of the specified camera.\n"
	       "\t--list|-l    : List available cameras.\n");

	exit(exit_code);
}

/* Arguments */

static const char *sopts = "c:hi:l";
static const struct option lopts[] = {
	{"capture", required_argument, NULL, 'c'},
	{"help",    no_argument,       NULL, 'h'},
	{"info",    required_argument, NULL, 'i'},
	{"list",    no_argument,       NULL, 'l'}
};

int main(int argc, char *argv[])
{
	int opt, rc = 0;

	/* Process Arguments */
	while ((opt = getopt_long(argc, argv, sopts, lopts, NULL)) != EOF) {
		switch (opt) {
		case 'c':
			rc = capture(optarg);
			break;
		case 'h':
			help(0);
			break;
		case 'i':
			rc = info(strtol(optarg, NULL, 0));
			break;
		case 'l':
			rc = list();
			break;
		default:
			break;
		}
	}

	if (rc)
		fprintf(stderr, "%s:%d: Error: %d\n", __FILE__, __LINE__, rc);

	return EXIT_SUCCESS;
}
