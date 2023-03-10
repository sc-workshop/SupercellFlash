#pragma once

#include <vector>

#include "SupercellFlash/flash/objects/DisplayObject.h"
#include "SupercellFlash/math/Rect.h"
#include "SupercellFlash/flash/objects/MovieClipFrame.h"

namespace sc
{
	class SupercellSWF;

	class MovieClip : public DisplayObject
	{
	public:
		MovieClip() { }
		virtual ~MovieClip() { }

	public:
		enum class Blend : uint8_t
		{
			Mix,
			MultiplyES = 3,
			Screen,
			Add = 8,
			Premultiply = 12
		};

	public:
		uint8_t frameRate() const { return m_frameRate; }
		void frameRate(uint8_t fps) { m_frameRate = fps; }

		std::vector<uint16_t>& instanceIds() { return m_instanceIds; }
		void instanceIds(const std::vector<uint16_t>& ids) { m_instanceIds = ids; }

		std::vector<Blend>& instanceBlends() { return m_instanceBlends; }
		void instanceBlends(const std::vector<Blend>& blends) { m_instanceBlends = blends; }

		std::vector<std::string>& instanceNames() { return m_instanceNames; }
		void instanceNames(const std::vector<std::string>& names) { m_instanceNames = names; }

		std::vector<MovieClipFrame>& frames() { return m_frames; }
		void frames(const std::vector<MovieClipFrame>& frames) { m_frames = frames; }

		Rect& scalingGrid() { return m_scalingGrid; }
		void scalingGrid(const Rect& grid) { m_scalingGrid = grid; }

		uint8_t matrixBankIndex() const { return m_matrixBankIndex; }
		void matrixBankIndex(uint8_t index) { m_matrixBankIndex = index; }

		bool isMovieClip() const override { return true; }

	public:
		void load(SupercellSWF* swf, uint8_t tag);

	private:
		uint8_t m_frameRate = 30;

		std::vector<uint16_t> m_instanceIds;
		std::vector<Blend> m_instanceBlends;
		std::vector<std::string> m_instanceNames;

		std::vector<MovieClipFrame> m_frames;

		Rect m_scalingGrid;
		uint8_t m_matrixBankIndex = 0;

		bool m_isTag35 = false; // it can be button or masks thing
	};
}
