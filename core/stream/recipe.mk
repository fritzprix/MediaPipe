INC-$(CONFIG_CORE)+=./core/stream
SRC-$(CONFIG_CORE)+=./core/stream
# add object to be built

OBJ-$(CONFIG_CORE)+= MediaClientSocketStream \
					MediaServerSocketStream \
					MediaFileStream \
					MediaStreamTestUnit
		
