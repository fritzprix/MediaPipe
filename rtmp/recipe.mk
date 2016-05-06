INC-$(CONFIG_RTMP)+=./rtmp
SRC-$(CONFIG_RTMP)+=./rtmp

OBJ-$(CONFIG_RTMP)+=FLVDemuxer \
		   FLVTag \
		   AMF0 \
		   FLVDumper\
		   FLVTestUnit
