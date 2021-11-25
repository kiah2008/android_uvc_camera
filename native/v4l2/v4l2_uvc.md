# v4l2

##  V4L2 API及数据结构

V4L2是V4L的升级版本，为linux下视频设备程序提供了一套接口规范。包括一套数据结构和底层V4L2驱动接口。

### 1、常用的结构体在内核目录**include/linux/videodev2.h**中定义

​	struct v4l2_requestbuffers    //申请帧缓冲，对应命令VIDIOC_REQBUFS 
​    struct v4l2_capability    //视频设备的功能，对应命令VIDIOC_QUERYCAP 
​    struct v4l2_input    //视频输入信息，对应命令VIDIOC_ENUMINPUT
​    struct v4l2_standard    //视频的制式，比如PAL，NTSC，对应命令VIDIOC_ENUMSTD 
​    struct v4l2_format    //帧的格式，对应命令VIDIOC_G_FMT、VIDIOC_S_FMT等
​    struct v4l2_buffer    //驱动中的一帧图像缓存，对应命令VIDIOC_QUERYBUF 
​    struct v4l2_crop    //视频信号矩形边框
​    v4l2_std_id    //视频制式

### 2、常用的IOCTL接口命令也在include/linux/videodev2.h中定义

​	VIDIOC_REQBUFS //分配内存 
​    VIDIOC_QUERYBUF     //把VIDIOC_REQBUFS中**分配的数据缓存转换成物理地址** 
​    VIDIOC_QUERYCAP    //查询驱动功能 
​    VIDIOC_ENUM_FMT    //获取当前驱动支持的视频格式 
​    VIDIOC_S_FMT    //设置当前驱动的频捕获格式 
​    VIDIOC_G_FMT    //读取当前驱动的频捕获格式 
​    VIDIOC_TRY_FMT    //验证当前驱动的显示格式 
​    VIDIOC_CROPCAP    //查询驱动的修剪能力 
​    VIDIOC_S_CROP    //设置视频信号的矩形边框 
​    VIDIOC_G_CROP    //读取视频信号的矩形边框
​    VIDIOC_QBUF    //把数据从缓存中读取出来 
​    VIDIOC_DQBUF    //把数据放回缓存队列 
​    VIDIOC_STREAMON    //开始视频显示函数 
​    VIDIOC_STREAMOFF    //结束视频显示函数 
​    VIDIOC_QUERYSTD     //检查当前视频设备支持的标准，例如PAL或NTSC。

### 3、操作流程

V4L2提供了很多访问接口，你可以根据具体需要选择操作方法。需要注意的是，很少有驱动完全实现了所有的接口功能。所以在使用时需要参考驱动源码，或仔细阅读驱动提供者的使用说明。

下面列举出一种操作的流程，供参考。

（1）打开设备文件

```c++
/*  Devicename：/dev/video0、/dev/video1 ……
    Mode：O_RDWR [| O_NONBLOCK]
    */
int fd = open(Devicename,mode);
```


如果使用非阻塞模式调用视频设备，则当没有可用的视频数据时，不会阻塞，而立刻返回。

（2）取得设备的capability
```c
struct v4l2_capability cap；
int ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
if (ret < 0) {
    printf("Error opening device %s: unable to query device.\n",
           vd->video_device);
    goto fatal;
}
if ((vd->cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == 0) {
    printf("Error opening device %s: video capture not supported.\n",
           vd->video_device);
    goto fatal;;
}

if (!(vd->cap.capabilities & V4L2_CAP_STREAMING)) {
     printf("%s does not support streaming i/o\n", vd->video_device);
     goto fatal;
 }
if (!(vd->cap.capabilities & V4L2_CAP_READWRITE)) {
    printf("%s does not support read i/o\n", vd->video_device);
    goto fatal;
 }

```

看看设备具有什么功能，比如是否具有视频输入特性。

（3）选择视频输入

```c
struct v4l2_input in;
memset(&in, 0, sizeof(in));
in.index=index;

if (v4l2_ioctl(dev, VIDIOC_ENUMINPUT, &in) < 0)
	return -1;
```

一个视频设备可以有多个视频输入。如果只有一路输入，这个功能可以没有。

（4）检测视频支持的制式

```c
v4l2_std_id std;
do {
   ret = ioctl(fd, VIDIOC_QUERYSTD, &std);
 } while (ret == -1 && errno == EAGAIN);

switch (std) {
   case V4L2_STD_NTSC: 
   //……
   case V4L2_STD_PAL:
   //……
}
```

（5）遍历视频支持格式，并设置视频捕获格式

```c
// enum format
struct v4l2_fmtdesc fmt;
memset(&fmt, 0, sizeof(fmt));
fmt.index = 0;
fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
while ((ret = ioctl(dev, VIDIOC_ENUM_FMT, &fmt)) == 0) {
    if (supported_formats == NULL) {
        printf("{ pixelformat = '%c%c%c%c', description = '%s' }\n",
               fmt.pixelformat & 0xFF, (fmt.pixelformat >> 8) & 0xFF,
               (fmt.pixelformat >> 16) & 0xFF, (fmt.pixelformat >> 24) & 0xFF,
               fmt.description);
        ret = enum_frame_sizes(dev, fmt.pixelformat);
        if (ret != 0)
            printf("  Unable to enumerate frame sizes.\n");
    } else if (fmt.index < max_formats) {
        supported_formats[fmt.index] = fmt.pixelformat;
    }
    fmt.index++;
}
// enum frame size
struct v4l2_frmsizeenum fsize;
memset(&fsize, 0, sizeof(fsize));
fsize.index = 0;
fsize.pixel_format = pixfmt;
while ((ret = ioctl(dev, VIDIOC_ENUM_FRAMESIZES, &fsize)) == 0) {
    if (fsize.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
        printf("{ discrete: width = %u, height = %u }\n",
               fsize.discrete.width, fsize.discrete.height);
        ret = enum_frame_intervals(dev, pixfmt,
                                   fsize.discrete.width, fsize.discrete.height);
        if (ret != 0)
            printf("  Unable to enumerate frame sizes.\n");
    } else if (fsize.type == V4L2_FRMSIZE_TYPE_CONTINUOUS) {
        printf("{ continuous: min { width = %u, height = %u } .. "
               "max { width = %u, height = %u } }\n",
               fsize.stepwise.min_width, fsize.stepwise.min_height,
               fsize.stepwise.max_width, fsize.stepwise.max_height);
        printf("  Refusing to enumerate frame intervals.\n");
        break;
    } else if (fsize.type == V4L2_FRMSIZE_TYPE_STEPWISE) {
        printf("{ stepwise: min { width = %u, height = %u } .. "
               "max { width = %u, height = %u } / "
               "stepsize { width = %u, height = %u } }\n",
               fsize.stepwise.min_width, fsize.stepwise.min_height,
               fsize.stepwise.max_width, fsize.stepwise.max_height,
               fsize.stepwise.step_width, fsize.stepwise.step_height);
        printf("  Refusing to enumerate frame intervals.\n");
        break;
    }
    fsize.index++;
}
//enum frame interval
struct v4l2_frmivalenum fival;
memset(&fival, 0, sizeof(fival));
fival.index = 0;
fival.pixel_format = pixfmt;
fival.width = width;
fival.height = height;
printf("\tTime interval between frame: ");
while ((ret = ioctl(dev, VIDIOC_ENUM_FRAMEINTERVALS, &fival)) == 0) {
    if (fival.type == V4L2_FRMIVAL_TYPE_DISCRETE) {
        printf("%u/%u, ",
               fival.discrete.numerator, fival.discrete.denominator);
    } else if (fival.type == V4L2_FRMIVAL_TYPE_CONTINUOUS) {
        printf("{min { %u/%u } .. max { %u/%u } }, ",
               fival.stepwise.min.numerator, fival.stepwise.min.numerator,
               fival.stepwise.max.denominator, fival.stepwise.max.denominator);
        break;
    } else if (fival.type == V4L2_FRMIVAL_TYPE_STEPWISE) {
        printf("{min { %u/%u } .. max { %u/%u } / "
               "stepsize { %u/%u } }, ",
               fival.stepwise.min.numerator, fival.stepwise.min.denominator,
               fival.stepwise.max.numerator, fival.stepwise.max.denominator,
               fival.stepwise.step.numerator, fival.stepwise.step.denominator);
        break;
    }
    fival.index++;
}

//================
struct v4l2_format fmt;
fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;//V4L2_BUF_TYPE_VIDEO_CAPTURE
fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY;
fmt.fmt.pix.height = height;
fmt.fmt.pix.width = width;
fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
ret = ioctl(fd, VIDIOC_S_FMT, &fmt);
if(ret) {
	perror("VIDIOC_S_FMT\n");
	close(fd);
	return -1;
}
//================
// Set pixel format and frame size
memset(&vd->fmt, 0, sizeof(struct v4l2_format));
vd->fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
vd->fmt.fmt.pix.width = vd->width;
vd->fmt.fmt.pix.height = vd->height;
vd->fmt.fmt.pix.pixelformat = vd->formatIn;
vd->fmt.fmt.pix.field = V4L2_FIELD_ANY;
ret = ioctl(vd->fd, VIDIOC_S_FMT, &vd->fmt);
if (ret < 0) {
    perror("Unable to set format");
    goto fatal;
}
if ((vd->fmt.fmt.pix.width != vd->width) ||
    (vd->fmt.fmt.pix.height != vd->height)) {
    printf("  Frame size:   %ux%u (requested size %ux%u is not supported by device)\n",
           vd->fmt.fmt.pix.width, vd->fmt.fmt.pix.height, vd->width, vd->height);
    vd->width = vd->fmt.fmt.pix.width;
    vd->height = vd->fmt.fmt.pix.height;
    /* look the format is not part of the deal ??? */
    //vd->formatIn = vd->fmt.fmt.pix.pixelformat;
} else {
    printf("  Frame size:   %dx%d\n", vd->width, vd->height);
}
```

（6）向驱动申请帧缓存

```c
/* request buffers */
#define NB_BUFFER 4
memset(&vd->rb, 0, sizeof(struct v4l2_requestbuffers));
vd->rb.count = NB_BUFFER;
vd->rb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
vd->rb.memory = V4L2_MEMORY_MMAP;
ret = ioctl(vd->fd, VIDIOC_REQBUFS, &vd->rb);
if (ret < 0) {
    perror("Unable to allocate buffers");
    goto fatal;
}
/* map the buffers */
for (i = 0; i < NB_BUFFER; i++) {
    memset(&vd->buf, 0, sizeof(struct v4l2_buffer));
    vd->buf.index = i;
    vd->buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    vd->buf.memory = V4L2_MEMORY_MMAP;
    ret = ioctl(vd->fd, VIDIOC_QUERYBUF, &vd->buf);
    if (ret < 0) {
        perror("Unable to query buffer");
        goto fatal;
    }
    if (debug)
        printf("length: %u offset: %u\n", vd->buf.length,
               vd->buf.m.offset);
    vd->mem[i] = mmap(0 /* start anywhere */ ,
                      vd->buf.length, PROT_READ, MAP_SHARED, vd->fd,
                      vd->buf.m.offset);
    if (vd->mem[i] == MAP_FAILED) {
        perror("Unable to map buffer");
        goto fatal;
    }
    if (debug)
        printf("Buffer mapped at address %p.\n", vd->mem[i]);
}
/* Queue the buffers. */
for (i = 0; i < NB_BUFFER; ++i) {
    memset(&vd->buf, 0, sizeof(struct v4l2_buffer));
    vd->buf.index = i;
    vd->buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    vd->buf.memory = V4L2_MEMORY_MMAP;
    ret = ioctl(vd->fd, VIDIOC_QBUF, &vd->buf);
    if (ret < 0) {
        perror("Unable to queue buffer");
        goto fatal;;
    }
}
```



v4l2_requestbuffers结构中定义了缓存的数量，驱动会据此申请对应数量的视频缓存。多个缓存可以用于建立FIFO，来提高视频采集的效率。

（7）获取每个缓存的信息，并mmap到用户空间

```c
typedef struct VideoBuffer {
	void *start;
	size_t length;
} VideoBuffer;

VideoBuffer* buffers = calloc( req.count, sizeof(*buffers) );
struct v4l2_buffer buf;

for (numBufs = 0; numBufs < req.count; numBufs++) {
    //映射所有的缓存
	memset( &buf, 0, sizeof(buf) );
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index = numBufs;
	if (ioctl(fd, VIDIOC_QUERYBUF, &buf) == -1) {
        //获取到对应index的缓存信息，此处主要利用length信息及offset信息来完成后面的mmap操作。
		return -1;
    }

	buffers[numBufs].length = buf.length;
	// 转换成相对地址
	buffers[numBufs].start = mmap(NULL, buf.length,PROT_READ | PROT_WRITE,MAP_SHARED,fd, buf.m.offset);
    if (buffers[numBufs].start == MAP_FAILED) {
            return -1;
    }
}
```





（8）开始采集视频

```c
int buf_type= V4L2_BUF_TYPE_VIDEO_CAPTURE；
int ret = ioctl(fd, VIDIOC_STREAMON, &buf_type);
```

（9）取出FIFO缓存中已经采样的帧缓存

```c
struct v4l2_buffer buf;
while (1) {
    memset(&vd->buf, 0, sizeof(struct v4l2_buffer));
    vd->buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    vd->buf.memory = V4L2_MEMORY_MMAP;
    ret = ioctl(vd->fd, VIDIOC_DQBUF, &vd->buf);
    if (ret < 0) {
        printf("Unable to dequeue buffer");
        exit(1);
    }
    //memcpy(vd->tmpbuffer, vd->mem[vd->buf.index], vd->buf.bytesused);
    //AVI_write_frame(vd->avifile, vd->tmpbuffer, vd->buf.bytesused, vd->framecount);
    vd->framecount++;
    ret = ioctl(vd->fd, VIDIOC_QBUF, &vd->buf);
    if (ret < 0) {
        printf("Unable to requeue buffer");
        exit(1);
    }
}
```

根据返回的buf.index找到对应的mmap映射好的缓存，取出视频数据。

（10）将刚刚处理完的缓冲重新入队列尾，这样可以循环采集

```c
if (ioctl(fd, VIDIOC_QBUF, &buf) == -1) {
	return -1;
}
```



（11）停止视频的采集

```c
int ret = ioctl(fd, VIDIOC_STREAMOFF, &buf_type);
```

（12）关闭视频设备

```c
close(fd);
```

### 4、 V4L2驱动框架

上述流程的各个操作都需要有底层V4L2驱动的支持。内核中有一些非常完善的例子。

比如：linux-2.6.26内核目录/drivers/media/video//zc301/zc301_core.c 中的ZC301视频驱动代码。上面的V4L2操作流程涉及的功能在其中都有实现。

1、V4L2驱动注册、注销函数

Video核心层（drivers/media/video/videodev.c）提供了注册函数
int video_register_device(struct video_device *vfd, int type, int nr)
video_device: 要构建的核心数据结构
Type: 表示设备类型，此设备号的基地址受此变量的影响
Nr: 如果end-base>nr>0 ：次设备号=base（基准值，受type影响）+nr；
否则：系统自动分配合适的次设备号

具体驱动只需要构建video_device结构，然后调用注册函数既可。

如：zc301_core.c中的
          err = video_register_device(cam->v4ldev, VFL_TYPE_GRABBER,
               video_nr[dev_nr]);
 Video核心层（drivers/media/video/videodev.c）提供了注销函数
	  void video_unregister_device(struct video_device *vfd)

2、struct video_device 的构建

video_device结构包含了视频设备的属性和操作方法。参见zc301_core.c

strcpy(cam->v4ldev->name, "ZC0301[P] PC Camera");
   cam->v4ldev->owner = THIS_MODULE;
   cam->v4ldev->type = VID_TYPE_CAPTURE | VID_TYPE_SCALES;
   **cam->v4ldev->fops = &zc0301_fops;**
   cam->v4ldev->minor = video_nr[dev_nr];
   cam->v4ldev->release = video_device_release;
   video_set_drvdata(cam->v4ldev, cam);

大家发现在这个zc301的驱动中并没有实现struct video_device中的很多操作函数，如：vidioc_querycap、vidioc_g_fmt_cap等。主要原因是struct file_operations zc0301_fops中的zc0301_ioctl实现了前面的所有ioctl操作。所以就不需要在struct video_device再实现struct video_device中的那些操作了。

另一种实现方法如下：

static struct video_device camif_dev =
{
	.name = "s3c2440 camif",
	.type = VID_TYPE_CAPTURE|VID_TYPE_SCALES|VID_TYPE_SUBCAPTURE,
	**.fops = &camif_fops,**
	.minor = -1,
	.release = camif_dev_release,
	.vidioc_querycap = vidioc_querycap,
	.vidioc_enum_fmt_cap = vidioc_enum_fmt_cap,
	.vidioc_g_fmt_cap = vidioc_g_fmt_cap,
	.vidioc_s_fmt_cap = vidioc_s_fmt_cap,
	.vidioc_queryctrl = vidioc_queryctrl,
	.vidioc_g_ctrl = vidioc_g_ctrl,
	.vidioc_s_ctrl = vidioc_s_ctrl,
};
static struct file_operations camif_fops =
{
	.owner = THIS_MODULE,
	.open = camif_open,
	.release = camif_release,
	.read = camif_read,
	.poll = camif_poll,
	**.ioctl = video_ioctl2, /\* V4L2 ioctl handler \*/**
	.mmap = camif_mmap,
	.llseek = no_llseek,
};

注意：video_ioctl2是videodev.c中是实现的。video_ioctl2中会根据ioctl不同的cmd来调用video_device中的操作方法。

3、Video核心层的实现

参见内核/drivers/media/videodev.c

（1）注册256个视频设备

static int __init videodev_init(void)
{
	int ret;
	if (**register_chrdev**(VIDEO_MAJOR, VIDEO_NAME, &video_fops)) {
		return -EIO;
	}
	ret = class_register(&video_class);
	……
}

上面的代码注册了256个视频设备，并注册了video_class类。video_fops为这256个设备共同的操作方法。

（2）V4L2驱动注册函数的实现

int video_register_device(struct video_device *vfd, int type, int nr)
{
	int i=0;
	int base;
	int end;
	int ret;
	char *name_base;

​    switch(type) //根据不同的type确定设备名称、次设备号
{
   **case VFL_TYPE_GRABBER:**
​	   **base=MINOR_VFL_TYPE_GRABBER_MIN;**
​	   **end=MINOR_VFL_TYPE_GRABBER_MAX+1;**
​	   **name_base = "video";**
​	   break;
   case VFL_TYPE_VTX:
​	   base=MINOR_VFL_TYPE_VTX_MIN;
​	   end=MINOR_VFL_TYPE_VTX_MAX+1;
​	   name_base = "vtx";
​	   break;
   case VFL_TYPE_VBI:
​	   base=MINOR_VFL_TYPE_VBI_MIN;
​	   end=MINOR_VFL_TYPE_VBI_MAX+1;
​	   name_base = "vbi";
​	   break;
   case VFL_TYPE_RADIO:
​	   base=MINOR_VFL_TYPE_RADIO_MIN;
​	   end=MINOR_VFL_TYPE_RADIO_MAX+1;
​	   name_base = "radio";
​	   break;
   default:
​	   printk(KERN_ERR "%s called with unknown type: %d\n",
​		   __func__, type);
​	   return -1;
}

​    /* 计算出次设备号 */
mutex_lock(&videodev_lock);
if (nr >= 0 && nr < end-base) {
   /* use the one the driver asked for */
   **i = base+nr;**
   if (NULL != video_device[i]) {
​	   mutex_unlock(&videodev_lock);
​	   return -ENFILE;
   }
} else {
   /* use first free */
   for(i=base;i<end;i++)
​	   if (NULL == video_device[i])
​		   break;
   if (i == end) {
​	   mutex_unlock(&videodev_lock);
​	   return -ENFILE;
   }
}
**video_device[i]=vfd; //保存video_device结构指针到系统的结构数组中，终的次设备号和i相关。**
vfd->minor=i;
mutex_unlock(&videodev_lock);
mutex_init(&vfd->lock);

​    /* sysfs class */
memset(&vfd->class_dev, 0x00, sizeof(vfd->class_dev));
if (vfd->dev)
   vfd->class_dev.parent = vfd->dev;
vfd->class_dev.class = &video_class;
vfd->class_dev.devt = MKDEV(VIDEO_MAJOR, vfd->minor);
**sprintf(vfd->class_dev.bus_id, "%s%d", name_base, i - base);//后在/dev目录下的名称**
**ret = device_register(&vfd->class_dev);//结合udev或mdev可以实现自动在/dev下创建设备节点**
……
​    }

从上面的注册函数中可以看出V4L2驱动的注册事实上只是完成了设备节点的创建，如：/dev/video0。和video_device结构指针的保存。

（3）视频驱动的打开过程

当用户空间调用open打开对应的视频文件时，如：

int fd = open(/dev/video0, O_RDWR);

对应/dev/video0的文件操作结构是/drivers/media/videodev.c中定义的video_fops。

static const struct file_operations video_fops=
{
	.owner = THIS_MODULE,
	.llseek = no_llseek,
	.open = video_open,
};

奇怪吧，这里只实现了open操作。那么后面的其它操作呢？还是先看看video_open吧。

static int video_open(struct inode *inode, struct file *file)
{
	unsigned int minor = iminor(inode);
	int err = 0;
	struct video_device *vfl;
	**const struct file_operations \*old_fops;**

​    if(minor>=VIDEO_NUM_DEVICES)
   return -ENODEV;
mutex_lock(&videodev_lock);
vfl=video_device[minor];
if(vfl==NULL) {
   mutex_unlock(&videodev_lock);
   request_module("char-major-%d-%d", VIDEO_MAJOR, minor);
   mutex_lock(&videodev_lock);
   **vfl=video_device[minor]; //根据次设备号取出video_device结构**
   if (vfl==NULL) {
​	   mutex_unlock(&videodev_lock);
​	   return -ENODEV;
   }
}
**old_fops = file->f_op;**
**file->f_op = fops_get(vfl->fops);//替换此打开文件的file_operation结构。后面的其它针对此文件的操作都由新的结构来负责了。也就是由每个具体的video_device的fops负责。**
if(file->f_op->open)
   err = file->f_op->open(inode,file);
if (err) {
   fops_put(file->f_op);
   file->f_op = fops_get(old_fops);
}
……
​    }

# uvc

# AVI


```

```