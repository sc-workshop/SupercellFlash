#pragma once

#include "SupercellFlash/objects/DisplayObject.h"
#include "SupercellFlash/objects/MovieClipFrame.h"

using namespace std;

namespace sc
{
	class SupercellSWF;

	struct MovieClipFrameElement
	{
		uint16_t instanceIndex;
		uint16_t matrixIndex = 0xFFFF;
		uint16_t colorTransformIndex = 0xFFFF;
	};
	typedef shared_ptr<MovieClipFrameElement> pMovieClipFrameElement;

	struct DisplayObjectInstance
	{
		enum class BlendMode : uint8_t
		{
			Mix,
			Layer,
			Darken,
			Multiply,
			Lighten,
			Screen,
			Overlay,
			Hardlight,
			Add,
			Subtract,
			Difference,
			Invert,
			Alpha,
			Erase
		};

		uint16_t id;
		BlendMode blend = BlendMode::Mix;
		string name;
	};
	typedef shared_ptr<DisplayObjectInstance> pDisplayObjectInstance;

	struct ScalingGrid
	{
		float x;
		float y;
		float width;
		float height;
	};
	typedef shared_ptr<ScalingGrid> pScalingGrid;

	class MovieClip : public DisplayObject
	{
	public:
		vector<pMovieClipFrameElement> frameElements;
		vector<pDisplayObjectInstance> instances;
		vector<pMovieClipFrame> frames;

	public:
		uint8_t frameRate() { return m_frameRate; }
		pScalingGrid scalingGrid() { return m_scalingGrid; }
		uint8_t matrixBankIndex() { return m_matrixBankIndex; }
		bool unknownFlag() { return m_unknownFlag; }

	public:
		void frameRate(uint8_t rate) { m_frameRate = rate; }
		void scalingGrid(pScalingGrid grid) { m_scalingGrid = grid; }
		void matrixBankIndex(uint8_t index) { m_matrixBankIndex = index; }
		void unknownFlag(bool status) { m_unknownFlag = status; }

	public:
		MovieClip* load(SupercellSWF* swf, uint8_t tag);
		void save(SupercellSWF* swf);

	private:
		uint8_t m_frameRate = 24;

		bool m_unknownFlag = false;
		pScalingGrid m_scalingGrid = nullptr;
		uint8_t m_matrixBankIndex = 0;

		uint8_t getTag();
	};

	typedef shared_ptr<MovieClip> pMovieClip;
}
