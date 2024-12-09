#include "MovieClipModifier.h"

#include "flash/objects/SupercellSWF.h"

namespace sc {
	namespace flash {
		void MovieClipModifier::load(SupercellSWF& swf, uint8_t tag)
		{
			id = swf.stream.read_unsigned_short();
			type = (Type)tag;
		}

		void MovieClipModifier::save(SupercellSWF& swf) const
		{
			swf.stream.write_unsigned_short(id);
		}

		uint8_t MovieClipModifier::tag(SupercellSWF&) const
		{
			return (uint8_t)type;
		}

		bool MovieClipModifier::is_modifier() const
		{
			return true;
		}

		void MovieClipModifier::load_sc2(SupercellSWF& swf, const SC2::DataStorage* storage, const uint8_t* data)
		{
			auto modifiers_data = SC2::GetMovieClipModifiers(data);

			auto modifiers_vector = modifiers_data->modifiers();
			if (!modifiers_vector) return;

			uint16_t modifiers_count = (uint16_t)modifiers_vector->size();
			swf.movieclip_modifiers.reserve(modifiers_count);

			for (uint16_t i = 0; modifiers_count > i; i++)
			{
				auto modifier_data = modifiers_vector->Get(i);
				MovieClipModifier& modifier = swf.movieclip_modifiers.emplace_back();

				modifier.id = modifier_data->id();
				modifier.type = (MovieClipModifier::Type)modifier_data->type();
			}
		}
	}
}