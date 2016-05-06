/*
 * FLVTestUnit.cpp
 *
 *  Created on: May 1, 2016
 *      Author: innocentevil
 */

#include "FLVTestUnit.h"

namespace MediaPipe {

FLVTestUnit::FLVTestUnit() {
	buffer = new uint32_t[1 << 18];
	filestream = new MediaFileStream("./data/fun_360_cube.flv");
	filestream->open();
	demuxer = new FLVDemuxer(filestream);
	dumpstream = new MediaFileStream("./out/fun_360_cube.h264");
	dumpstream->open();
	dumper = new FLVDumper(buffer,dumpstream);

	demuxer->setChunkHandler(dumper,FLVDemuxer::VIDEO);
}

FLVTestUnit::~FLVTestUnit() {
	filestream->close();
	dumpstream->close();
	delete filestream;
	delete dumpstream;
	delete demuxer;
	delete dumper;
	delete[] buffer;
}

bool FLVTestUnit::performTest() {
	return demuxer->demux();
}


} /* namespace MediaPipe */
