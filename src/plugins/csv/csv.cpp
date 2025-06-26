#include <v8.h>
#include <csv/csv.hpp>
#include <slim/plugin.hpp>
#include <vector>

namespace slim::csvdocument {
	void read_document_sync(const v8::FunctionCallbackInfo<v8::Value>& args);
}
void slim::csvdocument::read_document_sync(const v8::FunctionCallbackInfo<v8::Value>& args) {
    auto isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);
    auto context = isolate->GetCurrentContext();
	csv::CSVReader reader("/home/greergan/product/temp/GBG-export.csv");

	int row_index = 0;
	auto rows_array = v8::Array::New(isolate);
	for(auto& row: reader) {
		auto row_object = v8::Object::New(isolate);
		for(auto& column_name: row.get_col_names()) {
			auto result = row_object->DefineOwnProperty(
				context,
				slim::utilities::StringToName(isolate, column_name),
				slim::utilities::StringToV8Value(isolate, row[column_name].get())
			);
		}
		auto result = rows_array->Set(context, row_index, row_object);
		row_index++;
	}
	auto document = v8::Object::New(isolate);
	auto result = document->DefineOwnProperty(context, slim::utilities::StringToName(isolate, "contents"), rows_array);
	args.GetReturnValue().Set(document);
}
extern "C" void expose_plugin(v8::Isolate* isolate) {
	slim::plugin::plugin csv_plugin(isolate, "csv");
	csv_plugin.add_function("read_document_sync", slim::csvdocument::read_document_sync);
	csv_plugin.expose_plugin();
	return;
}