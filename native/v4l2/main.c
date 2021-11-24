
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <string.h>
#include <pthread.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <getopt.h>
#include "v4l2uvc.h"
#include "avilib.h"

static const char *usage = "bin"
                           "-s widthxheight"
                           "-d device/path"
                           "-a output name"
                           "-i fps"
                           "-h show this help message";

typedef struct capture_config_t {
    //see linux/videodev2.h, default as V4L2_PIX_FMT_MJPEG
    int format;
    int width;
    int height;
    //device path, default as "/dev/video0"
    const char *device_path;
    //avi output file
    const char *output_name;
    int fps;
};

int parse_args(int argc, char *argv[], struct capture_config_t *config) {
    char *sizestring = NULL;
    char *fpsstring = NULL;
    char *separateur = NULL;
    int fps;
    int i;
    int opt = 0;

    memset(config, 0, sizeof(*config));
    while ((opt = getopt(argc, argv, "hs:d:a:i:")) != -1) {
        switch (opt) {
            case 'h':
                printf("%s", usage);
                exit(0);
            case 's': {
                sizestring = strdup(optarg);
                config->width = strtoul(sizestring, &separateur, 10);
                if (*separateur != 'x') {
                    printf("Error in size use -s widthxheight\n");
                    exit(1);
                } else {
                    ++separateur;
                    config->height = strtoul(separateur, &separateur, 10);
                    if (*separateur != 0)
                        printf("hmm.. dont like that!! trying this height\n");
                }
                break;
            }
            case 'd':
                config->device_path = strdup(optarg);
                break;
            case 'a':
                config->output_name = strdup(optarg);
            case 'i':
                config->fps = atoi(optarg);
                break;
            default:
                printf("Unknown argments:\n %s", usage);
                return 1;
                break;
        }
    }

    if(config->format == 0) {
        config->format = V4L2_PIX_FMT_MJPEG;
    }
    if (config->device_path == NULL || *(config->device_path) == 0) {
        config->device_path = "/dev/video0";
    }

    if (config->output_name == NULL || *(config->output_name) == 0) {
        config->output_name = "test.avi";
    }
    return 0;
}

static struct capture_config_t capture_config = {0};

int main(int argc, char *argv[])   // -s 1280x720   -d  /dev/video1   -a  test.avi  -i 30
{
    FILE *file = NULL;
    struct vdIn *vd;
    unsigned char *tmpbuffer;

    char *sizestring = NULL;
    char *fpsstring = NULL;
    char *separateur = NULL;
    int width;
    int height;
    int fps;
    int i;

    parse_args(argc, argv, &capture_config);

    int format = V4L2_PIX_FMT_MJPEG;
    int ret;
    int grabmethod = 1;

    file = fopen(capture_config.device_path, "wb");
    if (file == NULL) {
        printf("Unable to open file for raw frame capturing\n ");
        exit(1);
    }

    //v4l2 init
    vd = (struct vdIn *) calloc(1, sizeof(struct vdIn));
    if (init_videoIn(vd, (char *) capture_config.device_path,  capture_config.width,
                     capture_config.height,  capture_config.fps,  capture_config.format,
                     grabmethod,  capture_config.output_name) <
        0) {
        exit(1);
    }

    if (video_enable(vd)) {
        exit(1);
    }

    vd->avifile = AVI_open_output_file(vd->avifilename);
    if (vd->avifile == NULL) {
        printf("Error opening avifile %s\n", vd->avifilename);
        exit(1);
    }

    AVI_set_video(vd->avifile, vd->width, vd->height, fps, "MJPG");
    printf("recording to %s\n", vd->avifilename);

    while (1) {
        memset(&vd->buf, 0, sizeof(struct v4l2_buffer));
        vd->buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        vd->buf.memory = V4L2_MEMORY_MMAP;
        ret = ioctl(vd->fd, VIDIOC_DQBUF, &vd->buf);
        if (ret < 0) {
            printf("Unable to dequeue buffer");
            exit(1);
        }

        memcpy(vd->tmpbuffer, vd->mem[vd->buf.index], vd->buf.bytesused);

        AVI_write_frame(vd->avifile, vd->tmpbuffer, vd->buf.bytesused, vd->framecount);

        vd->framecount++;

        ret = ioctl(vd->fd, VIDIOC_QBUF, &vd->buf);
        if (ret < 0) {
            printf("Unable to requeue buffer");
            exit(1);
        }
    }
    fclose(file);
    close_v4l2(vd);
}
