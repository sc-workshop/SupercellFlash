#include "ExportName.h"
#include "flash/objects/SupercellSWF.h"

namespace sc::flash
{
	void ExportName::load_sc2(SupercellSWF& swf, const SC2::DataStorage* storage, const uint8_t* data)
	{
		auto exports_data = SC2::GetExportNames(data);
		auto exports_ids = exports_data->object_ids();
		auto exports_name_ref_ids = exports_data->name_ref_ids();

		// Return if some of vectors are empty
		if (!exports_ids || !exports_name_ref_ids) return;
		if (exports_ids->size() != exports_name_ref_ids->size())
		{
			throw wk::Exception();
		}

		auto strings_vector = storage->strings();
		uint16_t export_names_count = (uint16_t)exports_ids->size();
		swf.exports.reserve(export_names_count);
		for (uint16_t i = 0; export_names_count > i; i++)
		{
			ExportName& export_name = swf.exports[i];

			SWFString name(
				strings_vector->Get(
					exports_name_ref_ids->Get(i)
				)->c_str()
			);

			swf.CreateExportName(name, exports_ids->Get(i));
		}
	}
}