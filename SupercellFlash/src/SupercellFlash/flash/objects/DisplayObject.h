#pragma once

#include <string>

namespace sc
{
	class DisplayObject
	{
	public:
		DisplayObject() { }
		virtual ~DisplayObject() { }

	public:
		uint16_t exportId() const { return m_exportId; }
		void exportId(uint16_t id) { m_exportId = id; }

		std::string exportName() const { return m_exportName; }
		void exportName(const std::string& name) { m_exportName = name; }

		virtual bool isMovieClip() const { return false; }
		virtual bool isShape() const { return false; }
		virtual bool isTextField() const { return false; }
		virtual bool isMovieClipModifier() const { return false; }

	protected:
		uint16_t m_exportId = 0;
		std::string m_exportName = "";
	};
}
