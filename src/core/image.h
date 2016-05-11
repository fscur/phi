#pragma once
#include <phi.h>

namespace phi
{
	namespace imageDataFormat
	{
		enum imageDataFormat
		{
			rgb,
			rgba,
			rgba8,
			bgr,
			bgra
		};
	}

	namespace imageDataType
	{
		enum imageDataType
		{
			ubyte_dataType,
			float_dataType
		};
	}

	struct image
	{
		uint w;
		uint h;
		imageDataFormat::imageDataFormat dataFormat;
		imageDataType::imageDataType dataType;
		byte* data;

		image(
			uint w,
			uint h,
			imageDataFormat::imageDataFormat dataFormat,
			imageDataType::imageDataType dataType,
			byte* data) :
			w(w),
			h(h),
			dataFormat(dataFormat),
			dataType(dataType),
			data(data)
		{
		}
	};
}