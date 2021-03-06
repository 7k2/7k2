/*
 * Seven Kingdoms 2: The Fryhtan War
 *
 * Copyright 2010 Unavowed <unavowed@vexillium.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef FILE_OUTPUT_STREAM_H
#define FILE_OUTPUT_STREAM_H

#include <ofile.h>
#include <output_stream.h>

class FileOutputStream: public OutputStream
{
private:
   File *file;

public:
   FileOutputStream();
   ~FileOutputStream();
   bool open(File *file);
   long write(const void *data, long length);
   bool seek(long offset, int whence);
   long tell();
   void close();
};

/* vim: set ts=8 sw=3: */
#endif
