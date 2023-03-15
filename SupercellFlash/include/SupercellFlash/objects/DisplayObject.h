#pragma once

#include <string>
#include <cstdint>

namespace sc
{
	class DisplayObject
	{
	public:
		DisplayObject() { }
		virtual ~DisplayObject() { }

	public:
		uint16_t id() const { return m_id; }
		void id(uint16_t id) { m_id = id; }

		std::string exportName() const { return m_exportName; }
		void exportName(const std::string& name) { m_exportName = name; }

		virtual bool isMovieClip() const { return false; }
		virtual bool isShape() const { return false; }
		virtual bool isTextField() const { return false; }
		virtual bool isMovieClipModifier() const { return false; }
		virtual bool hasExportName() const{ return false; }

	protected:
		uint16_t m_id = 0;
		std::string m_exportName = "";
	};
}
