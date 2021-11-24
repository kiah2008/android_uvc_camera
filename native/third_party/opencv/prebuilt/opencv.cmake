MESSAGE(STATUS "opencv.cmake current dir:${CMAKE_CURRENT_LIST_DIR}")

add_library(cpufeatures STATIC IMPORTED)
set_target_properties(cpufeatures PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libcpufeatures.a")

add_library(libjpeg-turbo STATIC IMPORTED)
set_target_properties(libjpeg-turbo PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/liblibjpeg-turbo.a")

add_library(libpng STATIC IMPORTED)
set_target_properties(libpng PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/liblibpng.a")

add_library(libprotobuf STATIC IMPORTED)
set_target_properties(libprotobuf PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/liblibprotobuf.a")

add_library(opencv_aruco STATIC IMPORTED)
set_target_properties(opencv_aruco PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_aruco.a")

add_library(opencv_bgsegm STATIC IMPORTED)
set_target_properties(opencv_bgsegm PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_bgsegm.a")

add_library(opencv_bioinspired STATIC IMPORTED)
set_target_properties(opencv_bioinspired PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_bioinspired.a")

add_library(opencv_calib3d STATIC IMPORTED)
set_target_properties(opencv_calib3d PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_calib3d.a")

add_library(opencv_ccalib STATIC IMPORTED)
set_target_properties(opencv_ccalib PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_ccalib.a")

add_library(opencv_core STATIC IMPORTED)
set_target_properties(opencv_core PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_core.a")

add_library(opencv_datasets STATIC IMPORTED)
set_target_properties(opencv_datasets PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_datasets.a")

add_library(opencv_dnn STATIC IMPORTED)
set_target_properties(opencv_dnn PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_dnn.a")

add_library(opencv_dnn_objdetect STATIC IMPORTED)
set_target_properties(opencv_dnn_objdetect PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_dnn_objdetect.a")

add_library(opencv_dnn_superres STATIC IMPORTED)
set_target_properties(opencv_dnn_superres PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_dnn_superres.a")

add_library(opencv_dpm STATIC IMPORTED)
set_target_properties(opencv_dpm PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_dpm.a")

add_library(opencv_face STATIC IMPORTED)
set_target_properties(opencv_face PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_face.a")

add_library(opencv_features2d STATIC IMPORTED)
set_target_properties(opencv_features2d PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_features2d.a")

add_library(opencv_flann STATIC IMPORTED)
set_target_properties(opencv_flann PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_flann.a")

add_library(opencv_fuzzy STATIC IMPORTED)
set_target_properties(opencv_fuzzy PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_fuzzy.a")

add_library(opencv_hfs STATIC IMPORTED)
set_target_properties(opencv_hfs PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_hfs.a")

add_library(opencv_highgui STATIC IMPORTED)
set_target_properties(opencv_highgui PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_highgui.a")

add_library(opencv_imgcodecs STATIC IMPORTED)
set_target_properties(opencv_imgcodecs PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_imgcodecs.a")

add_library(opencv_img_hash STATIC IMPORTED)
set_target_properties(opencv_img_hash PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_img_hash.a")

add_library(opencv_imgproc STATIC IMPORTED)
set_target_properties(opencv_imgproc PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_imgproc.a")

add_library(opencv_line_descriptor STATIC IMPORTED)
set_target_properties(opencv_line_descriptor PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_line_descriptor.a")

add_library(opencv_ml STATIC IMPORTED)
set_target_properties(opencv_ml PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_ml.a")

add_library(opencv_objdetect STATIC IMPORTED)
set_target_properties(opencv_objdetect PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_objdetect.a")

add_library(opencv_optflow STATIC IMPORTED)
set_target_properties(opencv_optflow PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_optflow.a")

add_library(opencv_phase_unwrapping STATIC IMPORTED)
set_target_properties(opencv_phase_unwrapping PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_phase_unwrapping.a")

add_library(opencv_photo STATIC IMPORTED)
set_target_properties(opencv_photo PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_photo.a")

add_library(opencv_plot STATIC IMPORTED)
set_target_properties(opencv_plot PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_plot.a")

add_library(opencv_quality STATIC IMPORTED)
set_target_properties(opencv_quality PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_quality.a")

add_library(opencv_reg STATIC IMPORTED)
set_target_properties(opencv_reg PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_reg.a")

add_library(opencv_rgbd STATIC IMPORTED)
set_target_properties(opencv_rgbd PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_rgbd.a")

add_library(opencv_saliency STATIC IMPORTED)
set_target_properties(opencv_saliency PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_saliency.a")

add_library(opencv_shape STATIC IMPORTED)
set_target_properties(opencv_shape PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_shape.a")

add_library(opencv_stereo STATIC IMPORTED)
set_target_properties(opencv_stereo PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_stereo.a")

add_library(opencv_stitching STATIC IMPORTED)
set_target_properties(opencv_stitching PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_stitching.a")

add_library(opencv_structured_light STATIC IMPORTED)
set_target_properties(opencv_structured_light PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_structured_light.a")

add_library(opencv_superres STATIC IMPORTED)
set_target_properties(opencv_superres PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_superres.a")

add_library(opencv_surface_matching STATIC IMPORTED)
set_target_properties(opencv_surface_matching PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_surface_matching.a")

add_library(opencv_text STATIC IMPORTED)
set_target_properties(opencv_text PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_text.a")

add_library(opencv_tracking STATIC IMPORTED)
set_target_properties(opencv_tracking PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_tracking.a")

add_library(opencv_video STATIC IMPORTED)
set_target_properties(opencv_video PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_video.a")

add_library(opencv_videoio STATIC IMPORTED)
set_target_properties(opencv_videoio PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_videoio.a")

add_library(opencv_videostab STATIC IMPORTED)
set_target_properties(opencv_videostab PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_videostab.a")

add_library(opencv_xfeatures2d STATIC IMPORTED)
set_target_properties(opencv_xfeatures2d PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_xfeatures2d.a")

add_library(opencv_ximgproc STATIC IMPORTED)
set_target_properties(opencv_ximgproc PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_ximgproc.a")

add_library(opencv_xobjdetect STATIC IMPORTED)
set_target_properties(opencv_xobjdetect PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_xobjdetect.a")

add_library(opencv_xphoto STATIC IMPORTED)
set_target_properties(opencv_xphoto PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libopencv_xphoto.a")

add_library(tbb STATIC IMPORTED)
set_target_properties(tbb PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libtbb.a")

add_library(tegra_hal STATIC IMPORTED)
set_target_properties(tegra_hal PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libtegra_hal.a")

set(opencv_INCLUDE_DIRS
        ${CMAKE_CURRENT_LIST_DIR}/include
        )

set(opencv_LIBRARIES
        opencv_imgcodecs
        opencv_imgproc
        opencv_video
        opencv_core
        cpufeatures
        opencv_features2d
        opencv_calib3d
        opencv_ccalib
        opencv_datasets
        opencv_dnn
        opencv_dnn_objdetect
        opencv_dnn_superres
        opencv_dpm
        opencv_face
        opencv_flann
        opencv_fuzzy
        opencv_hfs
        opencv_line_descriptor
        opencv_ml
        opencv_objdetect
        opencv_optflow
        opencv_phase_unwrapping
        opencv_photo
        opencv_plot
        opencv_quality
        opencv_reg
        opencv_rgbd
        opencv_saliency
        opencv_shape
        opencv_stereo
        opencv_stitching
        opencv_structured_light
        opencv_superres
        opencv_surface_matching
        opencv_text
        opencv_tracking
        opencv_videoio
        opencv_videostab
        opencv_xfeatures2d
        opencv_ximgproc
        opencv_xobjdetect
        opencv_xphoto
        tbb
        libpng
        libjpeg-turbo
        libprotobuf
        opencv_aruco
        opencv_bgsegm
        opencv_bioinspired
        tegra_hal
        )
