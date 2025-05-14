#include <cstdlib>
#include <v8.h>
#include <slim/common/log.h>
#include <slim/plugin.hpp>
#include <slim/utilities.h>
#include <simdutf.h>
#include <nbytes.h>
namespace slim::plugin::node {
	// BINARY is a deprecated alias of LATIN1.
	// BASE64URL is not currently exposed to the JavaScript side.
	enum encoding { ASCII, UTF8, BASE64, UCS2, BINARY, HEX, BUFFER, BASE64URL, LATIN1 = BINARY };
	// forward declaration
	template <typename T>
	T* UncheckedRealloc(T* pointer, size_t n);
	constexpr inline bool IsBigEndian() {
		return std::endian::native == std::endian::big;
	}
	template <typename T, size_t N>
	constexpr size_t arraysize(const T (&)[N]) {
		return N;
	}
	inline v8::Local<v8::String> OneByteString(v8::Isolate* isolate, const char* data, int length) {
		return v8::String::NewFromOneByte(isolate, reinterpret_cast<const uint8_t*>(data), v8::NewStringType::kNormal, length).ToLocalChecked();
	}
	inline v8::Local<v8::String> OneByteString(v8::Isolate* isolate, const signed char* data, int length) {
		return v8::String::NewFromOneByte(isolate, reinterpret_cast<const uint8_t*>(data), v8::NewStringType::kNormal, length).ToLocalChecked();
	}
	inline v8::Local<v8::String> OneByteString(v8::Isolate* isolate, const unsigned char* data, int length) {
		return v8::String::NewFromOneByte(isolate, data, v8::NewStringType::kNormal, length).ToLocalChecked();
	}
	inline v8::Local<v8::String> OneByteString(v8::Isolate* isolate, std::string_view str) {
		return OneByteString(isolate, str.data(), str.size());
	}
	void LowMemoryNotification() {
/* 		if(per_process::v8_initialized) {
			auto isolate = Isolate::TryGetCurrent();
		  	if(isolate != nullptr) {
				isolate->LowMemoryNotification();
		  	}
		} */
	}
	template <typename T>
	inline T MultiplyWithOverflowCheck(T a, T b) {
		auto ret = a * b;
		if(a != 0) {
			//CHECK_EQ(b, ret / a);
		}
	  	return ret;
	}
	// As per spec realloc behaves like malloc if passed nullptr.
	template <typename T>
	inline T* UncheckedMalloc(size_t n) {
  		return UncheckedRealloc<T>(nullptr, n);
	}
	// These should be used in our code as opposed to the native
	// versions as they abstract out some platform and or
	// compiler version specific functionality.
	// malloc(0) and realloc(ptr, 0) have implementation-defined behavior in
	// that the standard allows them to either return a unique pointer or a
	// nullptr for zero-sized allocation requests.  Normalize by always using
	// a nullptr.
	template <typename T>
	T* UncheckedRealloc(T* pointer, size_t n) {
  		size_t full_size = MultiplyWithOverflowCheck(sizeof(T), n);
  		if(full_size == 0) {
			free(pointer);
    		return nullptr;
  		}
  		void* allocated = realloc(pointer, full_size);
  		if(allocated == nullptr) [[unlikely]] {
    		// Tell V8 that memory is low and retry.
    		LowMemoryNotification();
    		allocated = realloc(pointer, full_size);
  		}
  		return static_cast<T*>(allocated);
	}
	template <typename T>
	inline T* Realloc(T* pointer, size_t n) {
  		T* ret = UncheckedRealloc(pointer, n);
  		//CHECK_IMPLIES(n > 0, ret != nullptr);
  		return ret;
	}
	template <typename T, size_t kStackStorageSize = 64>
	class ArrayBufferViewContents {
		public:
	  		ArrayBufferViewContents() = default;
	  		ArrayBufferViewContents(const ArrayBufferViewContents&) = delete;
	  		void operator=(const ArrayBufferViewContents&) = delete;
	  		explicit inline ArrayBufferViewContents(v8::Local<v8::Value> value);
	  		explicit inline ArrayBufferViewContents(v8::Local<v8::Object> value);
	  		explicit inline ArrayBufferViewContents(v8::Local<v8::ArrayBufferView> abv);
	  		inline void Read(v8::Local<v8::ArrayBufferView> abv);
	  		inline void ReadValue(v8::Local<v8::Value> buf);
	  		inline bool WasDetached() const { return was_detached_; }
	  		inline const T* data() const { return data_; }
	  		inline size_t length() const { return length_; }
	 	private:
		  	// Declaring operator new and delete as deleted is not spec compliant.
		  	// Therefore, declare them private instead to disable dynamic alloc.
	  		void* operator new(size_t size);
	  		void* operator new[](size_t size);
	  		void operator delete(void*, size_t);
	  		void operator delete[](void*, size_t);
	  		T stack_storage_[kStackStorageSize];
	  		T* data_ = nullptr;
	  		size_t length_ = 0;
	  		bool was_detached_ = false;
	};
	// Allocates an array of member type T. For up to kStackStorageSize items,
	// the stack is used, otherwise malloc().
	template <typename T, size_t kStackStorageSize = 1024>
	class MaybeStackBuffer {
 		public:
  			// Disallow copy constructor
  			MaybeStackBuffer(const MaybeStackBuffer&) = delete;
  			// Disallow copy assignment operator
  			MaybeStackBuffer& operator=(const MaybeStackBuffer& other) = delete;
  			const T* out() const {
    			return buf_;
  			}
  			T* out() {
    			return buf_;
  			}
  			// operator* for compatibility with `v8::String::(Utf8)Value`
			T* operator*() {
    			return buf_;
  			}
  			const T* operator*() const {
    			return buf_;
  			}
  			T& operator[](size_t index) {
    			CHECK_LT(index, length());
    			return buf_[index];
  			}
  			const T& operator[](size_t index) const {
    			CHECK_LT(index, length());
    			return buf_[index];
  			}
			size_t length() const {
    			return length_;
  			}
  			// Current maximum capacity of the buffer with which SetLength() can be used
  			// without first calling AllocateSufficientStorage().
  			size_t capacity() const {
    			return capacity_;
  			}
  			// Make sure enough space for `storage` entries is available.
  			// This method can be called multiple times throughout the lifetime of the
  			// buffer, but once this has been called Invalidate() cannot be used.
  			// Content of the buffer in the range [0, length()) is preserved.
  			//void AllocateSufficientStorage(size_t storage);
			void AllocateSufficientStorage(size_t storage) {
				//CHECK(!IsInvalidated());
				if(storage > capacity()) {
					bool was_allocated = IsAllocated();
					T* allocated_ptr = was_allocated ? buf_ : nullptr;
					buf_ = Realloc(allocated_ptr, storage);
					capacity_ = storage;
					if (!was_allocated && length_ > 0)
					memcpy(buf_, buf_st_, length_ * sizeof(buf_[0]));
				}
				length_ = storage;
			} 
  			void SetLength(size_t length) {
    			// capacity() returns how much memory is actually available.
    			//CHECK_LE(length, capacity());
    			length_ = length;
  			}
  			void SetLengthAndZeroTerminate(size_t length) {
    			// capacity() returns how much memory is actually available.
    			//CHECK_LE(length + 1, capacity());
    			SetLength(length);
    			// T() is 0 for integer types, nullptr for pointers, etc.
    			buf_[length] = T();
  			}
  			// Make dereferencing this object return nullptr.
  			// This method can be called multiple times throughout the lifetime of the
  			// buffer, but once this has been called AllocateSufficientStorage() cannot
  			// be used.
  			void Invalidate() {
    			//CHECK(!IsAllocated());
    			capacity_ = 0;
    			length_ = 0;
    			buf_ = nullptr;
  			}
  			// If the buffer is stored in the heap rather than on the stack.
  			bool IsAllocated() const {
    			return !IsInvalidated() && buf_ != buf_st_;
  			}
  			// If Invalidate() has been called.
  			bool IsInvalidated() const {
    			return buf_ == nullptr;
  			}
  			// Release ownership of the malloc'd buffer.
  			// Note: This does not free the buffer.
  			void Release() {
    			//CHECK(IsAllocated());
    			buf_ = buf_st_;
    			length_ = 0;
    			capacity_ = arraysize(buf_st_);
  			}
  			MaybeStackBuffer() : length_(0), capacity_(arraysize(buf_st_)), buf_(buf_st_) {
    			// Default to a zero-length, null-terminated buffer.
    			buf_[0] = T();
  			}
  			explicit MaybeStackBuffer(size_t storage) : MaybeStackBuffer() {
    			AllocateSufficientStorage(storage);
  			}
  			~MaybeStackBuffer() {
    			if(IsAllocated())
      				free(buf_);
  			}
  			inline std::basic_string<T> ToString() const {
				return {out(), length()};
			}
  			inline std::basic_string_view<T> ToStringView() const {
    			return {out(), length()};
  			}
 		private:
  			size_t length_;
  			// capacity of the malloc'ed buf_
  			size_t capacity_;
  			T* buf_;
  			T buf_st_[kStackStorageSize];
	};
};
namespace slim::plugin::node::Buffer {
	bool HasInstance(v8::Local<v8::Value> val);
	size_t Length(v8::Local<v8::Value> val);
	static void Atob(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void Btoa(const v8::FunctionCallbackInfo<v8::Value>& args);
	void Compare(const v8::FunctionCallbackInfo<v8::Value>& args);
	void CompareOffset(const v8::FunctionCallbackInfo<v8::Value>& args);
	void fill(const v8::FunctionCallbackInfo<v8::Value>& args);
	void isAscii(const v8::FunctionCallbackInfo<v8::Value>& args);
	void isUtf8(const v8::FunctionCallbackInfo<v8::Value>& args);
	void IndexOfBuffer(const v8::FunctionCallbackInfo<v8::Value>& args);
	void IndexOfString(const v8::FunctionCallbackInfo<v8::Value>& args);
	void SlowIndexOfNumber(const v8::FunctionCallbackInfo<v8::Value>& args);
	void SlowByteLengthUtf8(const v8::FunctionCallbackInfo<v8::Value>& args);
	void SlowCopy(const v8::FunctionCallbackInfo<v8::Value>& args);
	void Swap16(const v8::FunctionCallbackInfo<v8::Value>& args);
	void Swap32(const v8::FunctionCallbackInfo<v8::Value>& args);
	void Swap64(const v8::FunctionCallbackInfo<v8::Value>& args);
	int32_t IndexOfNumber(const uint8_t* buffer_data, size_t buffer_length, uint32_t needle, int64_t offset_i64, bool is_forward);
	static int normalizeCompareVal(int val, size_t a_length, size_t b_length);
};
namespace slim::plugin::node::internal::buffer {
/* 	FastBuffer,
	markAsUntransferable,
	addBufferPrototypeMethods,
	createUnsafeBuffer, */
}
namespace slim::plugin::node::internal::errors {
/* 	codes: {
		ERR_BUFFER_OUT_OF_BOUNDS,
		ERR_INVALID_ARG_TYPE,
		ERR_INVALID_ARG_VALUE,
		ERR_INVALID_BUFFER_SIZE,
		ERR_MISSING_ARGS,
		ERR_OUT_OF_RANGE,
		ERR_UNKNOWN_ENCODING,
	  },
	  genericNodeError, */
}
namespace slim::plugin::node::internal::util {
	static void GetOwnNonIndexProperties(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void IsInsideNodeModules(const v8::FunctionCallbackInfo<v8::Value>& args);
}
namespace slim::plugin::node::internal::util::inspect {
	// inspect
}
namespace slim::plugin::node::internal::util::types {
/* 	isAnyArrayBuffer,
	isArrayBufferView,
	isUint8Array,
	isTypedArray, */
}
namespace slim::plugin::node::internal::validators {
/* 	validateArray,
	validateBuffer,
	validateInteger,
	validateNumber,
	validateString, */
}
namespace slim::plugin::node::util {

}
namespace slim::plugin::node::StringBytes {
	v8::Maybe<size_t> Size(v8::Isolate* isolate, v8::Local<v8::Value> val, enum encoding encoding);
	class StringBytes {
		public:
		 	class InlineDecoder : public MaybeStackBuffer<char> {
		  		public:
		   			inline v8::Maybe<void> Decode(v8::Isolate* isolate, v8::Local<v8::String> string, enum encoding enc) {
			 			size_t storage;
			 			if(!StringBytes::StorageSize(isolate, string, enc).To(&storage)) {
							return v8::Nothing<void>();
						}
			 			AllocateSufficientStorage(storage);
			 			const size_t length = StringBytes::Write(isolate, out(), storage, string, enc);
			 			// No zero terminator is included when using this method.
			 			SetLength(length);
			 			return v8::JustVoid();
		   			}
		   			inline size_t size() const {
						return length();
					}
		 	};
		 	// Fast, but can be 2 bytes oversized for Base64, and
		 	// as much as triple UTF-8 strings <= 65536 chars in length
		 	static v8::Maybe<size_t> StorageSize(v8::Isolate* isolate, v8::Local<v8::Value> val, enum encoding enc);
		 	// Precise byte count, but slightly slower for Base64 and
		 	// very much slower for UTF-8
		 	static v8::Maybe<size_t> Size(v8::Isolate* isolate, v8::Local<v8::Value> val, enum encoding enc);
		 	// Write the bytes from the string or buffer into the char*
		 	// returns the number of bytes written, which will always be
		 	// <= buflen.  Use StorageSize/Size first to know how much
		 	// memory to allocate.
		 	static size_t Write(v8::Isolate* isolate, char* buf, size_t buflen, v8::Local<v8::Value> val, enum encoding enc);
		 	// Take the bytes in the src, and turn it into a Buffer or String.
		 	static v8::MaybeLocal<v8::Value> Encode(v8::Isolate* isolate, const char* buf, size_t buflen, enum encoding encoding);
		 	// Warning: This reverses endianness on BE platforms, even though the
		 	// signature using uint16_t implies that it should not.
		 	// However, the brokenness is already public API and can't therefore
		 	// be changed easily.
		 	static v8::MaybeLocal<v8::Value> Encode(v8::Isolate* isolate, const uint16_t* buf, size_t buflen);
		 	static v8::MaybeLocal<v8::Value> Encode(v8::Isolate* isolate, const char* buf, enum encoding encoding);	   
		private:
			static size_t WriteUCS2(v8::Isolate* isolate, char* buf, size_t buflen, v8::Local<v8::String> str, int flags);
	};
};
extern "C" void expose_plugin(v8::Isolate* isolate) {
	int ALL_PROPERTIES = v8::PropertyFilter::ALL_PROPERTIES;
	int ONLY_ENUMERABLE = v8::PropertyFilter::ONLY_ENUMERABLE;
	slim::plugin::plugin node_plugin(isolate, "node");
	slim::plugin::plugin node_buffer_plugin(isolate, "buffer");
	slim::plugin::plugin node_internal_plugin(isolate, "internal");
	slim::plugin::plugin node_internal_buffer_plugin(isolate, "buffer");
	slim::plugin::plugin node_internal_errors_plugin(isolate, "errors");
	slim::plugin::plugin node_internal_errors_codes_plugin(isolate, "codes");
	slim::plugin::plugin node_internal_util_plugin(isolate, "util");
	slim::plugin::plugin node_internal_util_types_plugin(isolate, "types");
	slim::plugin::plugin node_internal_util_inspect_plugin(isolate, "inspect");
	slim::plugin::plugin node_internal_validators_plugin(isolate, "validators");
//slim::plugin::plugin node_util_plugin(isolate, "util");
	slim::plugin::plugin node_internal_util_constants_plugin(isolate, "constants");
	node_buffer_plugin.add_property_immutable("kMaxLength", v8::Uint8Array::kMaxLength);
	node_buffer_plugin.add_property_immutable("kStringMaxLength", v8::String::kMaxLength);
	node_buffer_plugin.add_function("atob", slim::plugin::node::Buffer::Atob);
	node_buffer_plugin.add_function("btoa", slim::plugin::node::Buffer::Btoa);
	node_buffer_plugin.add_function("byteLengthUtf8", slim::plugin::node::Buffer::SlowByteLengthUtf8);
	node_buffer_plugin.add_function("compare", slim::plugin::node::Buffer::Compare);
	node_buffer_plugin.add_function("compareOffset", slim::plugin::node::Buffer::CompareOffset);
	node_buffer_plugin.add_function("copy", slim::plugin::node::Buffer::SlowCopy);
	node_buffer_plugin.add_function("indexOfBuffer", slim::plugin::node::Buffer::IndexOfBuffer);
	node_buffer_plugin.add_function("indexOfNumber", slim::plugin::node::Buffer::SlowIndexOfNumber);
	node_buffer_plugin.add_function("indexOfString", slim::plugin::node::Buffer::IndexOfString);
	node_buffer_plugin.add_function("swap16", slim::plugin::node::Buffer::Swap16);
	node_buffer_plugin.add_function("swap32", slim::plugin::node::Buffer::Swap32);
	node_buffer_plugin.add_function("swap64", slim::plugin::node::Buffer::Swap64);

	//node_internal_util_inspect_plugin
	//node_internal_util_types_plugin
	//node_internal_util_plugin

	//node_internal_validators_plugin
	//node_internal_errors_plugin
	//node_internal_buffers_plugin

	node_internal_util_plugin.add_function("getOwnNonIndexProperties", slim::plugin::node::internal::util::GetOwnNonIndexProperties);
	node_internal_util_plugin.add_function("isInsideNodeModules", slim::plugin::node::internal::util::IsInsideNodeModules);
	node_internal_util_constants_plugin.add_property_immutable("ALL_PROPERTIES", ALL_PROPERTIES);
	node_internal_util_constants_plugin.add_property_immutable("ONLY_ENUMERABLE", ONLY_ENUMERABLE);
	node_internal_errors_plugin.add_plugin("codes", &node_internal_errors_codes_plugin);
	node_internal_plugin.add_plugin("buffer", &node_internal_buffer_plugin);
	node_internal_plugin.add_plugin("errors", &node_internal_errors_plugin);
	node_internal_plugin.add_plugin("validators", &node_internal_validators_plugin);

	node_internal_util_plugin.add_plugin("inspect", &node_internal_util_inspect_plugin);
	node_internal_util_plugin.add_plugin("types", &node_internal_util_types_plugin);
	node_internal_plugin.add_plugin("util", &node_internal_util_plugin);
	node_internal_util_plugin.add_plugin("constants", &node_internal_util_constants_plugin);
	node_plugin.add_plugin("buffer", &node_buffer_plugin);
	node_plugin.add_plugin("internal", &node_internal_plugin);
//node_plugin.add_plugin("util", &node_util_plugin);
	node_plugin.expose_plugin();
	return;
};
template <typename T, size_t S>
slim::plugin::node::ArrayBufferViewContents<T, S>::ArrayBufferViewContents(v8::Local<v8::Value> value) {
	//DCHECK(value->IsArrayBufferView() || value->IsSharedArrayBuffer() || value->IsArrayBuffer());
  	ReadValue(value);
}
template <typename T, size_t S>
slim::plugin::node::ArrayBufferViewContents<T, S>::ArrayBufferViewContents(v8::Local<v8::Object> value) {
	//CHECK(value->IsArrayBufferView());
  	Read(value.As<v8::ArrayBufferView>());
}
template <typename T, size_t S>
slim::plugin::node::ArrayBufferViewContents<T, S>::ArrayBufferViewContents(v8::Local<v8::ArrayBufferView> abv) {
	Read(abv);
}
template <typename T, size_t S>
void slim::plugin::node::ArrayBufferViewContents<T, S>::Read(v8::Local<v8::ArrayBufferView> abv) {
	static_assert(sizeof(T) == 1, "Only supports one-byte data at the moment");
	length_ = abv->ByteLength();
	if(length_ > sizeof(stack_storage_) || abv->HasBuffer()) {
    	data_ = static_cast<T*>(abv->Buffer()->Data()) + abv->ByteOffset();
  	}
	else {
    	abv->CopyContents(stack_storage_, sizeof(stack_storage_));
    	data_ = stack_storage_;
  	}
}
template <typename T, size_t S>
void slim::plugin::node::ArrayBufferViewContents<T, S>::ReadValue(v8::Local<v8::Value> buf) {
	static_assert(sizeof(T) == 1, "Only supports one-byte data at the moment");
  	//DCHECK(buf->IsArrayBufferView() || buf->IsSharedArrayBuffer() || buf->IsArrayBuffer());
	if(buf->IsArrayBufferView()) {
    	Read(buf.As<v8::ArrayBufferView>());
  	}
	else if(buf->IsArrayBuffer()) {
    	auto ab = buf.As<v8::ArrayBuffer>();
    	length_ = ab->ByteLength();
    	data_ = static_cast<T*>(ab->Data());
    	was_detached_ = ab->WasDetached();
  	}
  	else {
		//CHECK(buf->IsSharedArrayBuffer());
    	auto sab = buf.As<v8::SharedArrayBuffer>();
    	length_ = sab->ByteLength();
    	data_ = static_cast<T*>(sab->Data());
  	}
}
// Computes the offset for starting an indexOf or lastIndexOf search.
// Returns either a valid offset in [0...<length - 1>], ie inside the Buffer,
// or -1 to signal that there is no possible match.
int64_t IndexOfOffset(size_t length, int64_t offset_i64, int64_t needle_length, bool is_forward) {
	int64_t length_i64 = static_cast<int64_t>(length);
	if(offset_i64 < 0) {
		if(offset_i64 + length_i64 >= 0) {
			// Negative offsets count backwards from the end of the buffer.
			return length_i64 + offset_i64;
		}
		else if(is_forward || needle_length == 0) {
			// indexOf from before the start of the buffer: search the whole buffer.
			return 0;
		}
		else {
			// lastIndexOf from before the start of the buffer: no match.
			return -1;
		}
	}
	else {
		if(offset_i64 + needle_length <= length_i64) {
		// Valid positive offset.
			return offset_i64;
		}
		else if(needle_length == 0) {
			// Out of buffer bounds, but empty needle: point to end of buffer.
			return length_i64;
		}
		else if(is_forward) {
			// indexOf from past the end of the buffer: no match.
			return -1;
		}
		else {
			// lastIndexOf from past the end of the buffer: search the whole buffer.
			return length_i64 - 1;
		}
	}
}
int32_t slim::plugin::node::Buffer::IndexOfNumber(const uint8_t* buffer_data, size_t buffer_length, uint32_t needle, int64_t offset_i64, bool is_forward) {
	int64_t opt_offset = IndexOfOffset(buffer_length, offset_i64, 1, is_forward);
	if(opt_offset <= -1 || buffer_length == 0) {
		return -1;
	}
	size_t offset = static_cast<size_t>(opt_offset);
	//CHECK_LT(offset, buffer_length);
	const void* ptr;
	if(is_forward) {
		ptr = memchr(buffer_data + offset, needle, buffer_length - offset);
	}
	else {
		ptr = nbytes::stringsearch::MemrchrFill(buffer_data, needle, offset + 1);
	}
	const uint8_t* ptr_uint8 = static_cast<const uint8_t*>(ptr);
	return ptr != nullptr ? static_cast<int32_t>(ptr_uint8 - buffer_data) : -1;
}
// Normalize val to be an integer in the range of [1, -1] since
// implementations of memcmp() can vary by platform.
static int slim::plugin::node::Buffer::normalizeCompareVal(int val, size_t a_length, size_t b_length) {
	if(val == 0) {
		return a_length > b_length ? 1 : -1;
	}
	else {
		return val > 0 ? 1 : -1;
	}
	return val;
}
// In case of success, the decoded string is returned.
// In case of error, a negative value is returned:
// * -1 indicates a single character remained,
// * -2 indicates an invalid character,
// * -3 indicates a possible overflow (i.e., more than 2 GB output).
static void slim::plugin::node::Buffer::Atob(const v8::FunctionCallbackInfo<v8::Value>& args) {
/* 	CHECK_EQ(args.Length(), 1);
	Environment* env = Environment::GetCurrent(args);
	THROW_AND_RETURN_IF_NOT_STRING(env, args[0], "argument"); */
	auto* isolate = args.GetIsolate();
	v8::Local<v8::String> input = args[0].As<v8::String>();
	MaybeStackBuffer<char> buffer;
	simdutf::result result;
	if(input->IsExternalOneByte()) {  // 8-bit case
		auto ext = input->GetExternalOneByteStringResource();
		size_t expected_length = simdutf::maximal_binary_length_from_base64(ext->data(), ext->length());
	  	buffer.AllocateSufficientStorage(expected_length);
	  	buffer.SetLength(expected_length);
	  	result = simdutf::base64_to_binary(ext->data(), ext->length(), buffer.out(), simdutf::base64_default);
	}
	else if(input->IsOneByte()) {
	  	MaybeStackBuffer<uint8_t> stack_buf(input->Length());
	  	input->WriteOneByte(args.GetIsolate(), stack_buf.out(), 0, input->Length(), v8::String::NO_NULL_TERMINATION);
	  	const char* data = reinterpret_cast<const char*>(*stack_buf);
	  	size_t expected_length = simdutf::maximal_binary_length_from_base64(data, input->Length());
	  	buffer.AllocateSufficientStorage(expected_length);
	  	buffer.SetLength(expected_length);
	  	result = simdutf::base64_to_binary(data, input->Length(), buffer.out());
	}
	else {  // 16-bit case
	  	v8::String::Value value(isolate, input);
	  	auto data = reinterpret_cast<const char16_t*>(*value);
	  	size_t expected_length = simdutf::maximal_binary_length_from_base64(data, value.length());
	  	buffer.AllocateSufficientStorage(expected_length);
	  	buffer.SetLength(expected_length);
	  	result = simdutf::base64_to_binary(data, value.length(), buffer.out());
	}
	if(result.error == simdutf::error_code::SUCCESS) {
	  	auto value = slim::plugin::node::OneByteString(isolate, reinterpret_cast<const uint8_t*>(buffer.out()), result.count);
	  	return args.GetReturnValue().Set(value);
	}
	// Default value is: "possible overflow"
	int32_t error_code = -3;
	if(result.error == simdutf::error_code::INVALID_BASE64_CHARACTER) {
	  	error_code = -2;
	}
	else if(result.error == simdutf::error_code::BASE64_INPUT_REMAINDER) {
	  	error_code = -1;
	}
	args.GetReturnValue().Set(error_code);
}
static void slim::plugin::node::Buffer::Btoa(const v8::FunctionCallbackInfo<v8::Value>& args) {
	auto* isolate = args.GetIsolate();
/* 	CHECK_EQ(args.Length(), 1);
	Environment* env = Environment::GetCurrent(args);
	THROW_AND_RETURN_IF_NOT_STRING(env, args[0], "argument"); */
	v8::Local<v8::String> input = args[0].As<v8::String>();
	MaybeStackBuffer<char> buffer;
	size_t written;
	if(input->IsExternalOneByte()) {  // 8-bit case
		auto ext = input->GetExternalOneByteStringResource();
	  	size_t expected_length = simdutf::base64_length_from_binary(ext->length());
	  	buffer.AllocateSufficientStorage(expected_length + 1);
	  	buffer.SetLengthAndZeroTerminate(expected_length);
	  	written = simdutf::binary_to_base64(ext->data(), ext->length(), buffer.out());
	}
	else if(input->IsOneByte()) {
		MaybeStackBuffer<uint8_t> stack_buf(input->Length());
	  	input->WriteOneByte(isolate, stack_buf.out(), 0, input->Length(), v8::String::NO_NULL_TERMINATION);
	  	size_t expected_length = simdutf::base64_length_from_binary(input->Length());
		buffer.AllocateSufficientStorage(expected_length + 1);
	  	buffer.SetLengthAndZeroTerminate(expected_length);
	  	written = simdutf::binary_to_base64(reinterpret_cast<const char*>(*stack_buf), input->Length(), buffer.out());
	}
	else {
		v8::String::Value value(isolate, input);
	  	MaybeStackBuffer<char> stack_buf(value.length());
	  	size_t out_len = simdutf::convert_utf16_to_latin1(reinterpret_cast<const char16_t*>(*value), value.length(), stack_buf.out());
	  	if(out_len == 0) {  // error
			return args.GetReturnValue().Set(-1);
	  	}
	  	size_t expected_length = simdutf::base64_length_from_binary(out_len);
	  	buffer.AllocateSufficientStorage(expected_length + 1);
	  	buffer.SetLengthAndZeroTerminate(expected_length);
	  	written = simdutf::binary_to_base64(*stack_buf, out_len, buffer.out());
	}
	auto value = OneByteString(isolate, reinterpret_cast<const uint8_t*>(buffer.out()), written);
	return args.GetReturnValue().Set(value);
}
void slim::plugin::node::Buffer::SlowByteLengthUtf8(const v8::FunctionCallbackInfo<v8::Value>& args) {
	auto* isolate = args.GetIsolate();
	//Environment* env = Environment::GetCurrent(args);
	//CHECK(args[0]->IsString());
	// Fast case: avoid StringBytes on UTF8 string. Jump to v8.
	args.GetReturnValue().Set(args[0].As<v8::String>()->Utf8Length(isolate));
}
void slim::plugin::node::Buffer::Compare(const v8::FunctionCallbackInfo<v8::Value> &args) {
/* 	Environment* env = Environment::GetCurrent(args);
	THROW_AND_RETURN_UNLESS_BUFFER(env, args[0]);
	THROW_AND_RETURN_UNLESS_BUFFER(env, args[1]); */
	ArrayBufferViewContents<char> a(args[0]);
	ArrayBufferViewContents<char> b(args[1]);
	size_t cmp_length = std::min(a.length(), b.length());
	int val = normalizeCompareVal(cmp_length > 0 ? memcmp(a.data(), b.data(), cmp_length) : 0, a.length(), b.length());
	args.GetReturnValue().Set(val);
}
void slim::plugin::node::Buffer::CompareOffset(const v8::FunctionCallbackInfo<v8::Value> &args) {
/* 	Environment* env = Environment::GetCurrent(args);
	THROW_AND_RETURN_UNLESS_BUFFER(env, args[0]);
	THROW_AND_RETURN_UNLESS_BUFFER(env, args[1]); */
	ArrayBufferViewContents<char> source(args[0]);
	ArrayBufferViewContents<char> target(args[1]);
	size_t target_start = 0;
	size_t source_start = 0;
	size_t source_end = 0;
	size_t target_end = 0;
/* 	THROW_AND_RETURN_IF_OOB(ParseArrayIndex(env, args[2], 0, &target_start));
	THROW_AND_RETURN_IF_OOB(ParseArrayIndex(env, args[3], 0, &source_start));
	THROW_AND_RETURN_IF_OOB(ParseArrayIndex(env, args[4], target.length(), &target_end));
	THROW_AND_RETURN_IF_OOB(ParseArrayIndex(env, args[5], source.length(), &source_end)); */
	if(source_start > source.length()) {
		//return THROW_ERR_OUT_OF_RANGE(env, "The value of \"sourceStart\" is out of range.");
	}
	if (target_start > target.length()) {
		//return THROW_ERR_OUT_OF_RANGE(env, "The value of \"targetStart\" is out of range.");
	}
/* 	CHECK_LE(source_start, source_end);
	CHECK_LE(target_start, target_end); */
	size_t to_cmp = std::min(std::min(source_end - source_start, target_end - target_start), source.length() - source_start);
	int val = normalizeCompareVal(to_cmp > 0 
									? memcmp(source.data() + source_start, target.data() + target_start,to_cmp)
									: 0, 
									source_end - source_start, target_end - target_start);
	args.GetReturnValue().Set(val);
}
// Assume caller has properly validated args.
void slim::plugin::node::Buffer::SlowCopy(const v8::FunctionCallbackInfo<v8::Value>& args) {
	auto context = args.GetIsolate()->GetCurrentContext();
	ArrayBufferViewContents<char> source(args[0]);
	//SPREAD_BUFFER_ARG(args[1].As<Object>(), target);
	v8::Local<v8::ArrayBufferView> target = args[1].As<v8::ArrayBufferView>();
	const size_t target_offset = target->ByteOffset();
	const size_t target_length = target->ByteLength();
	if(target_length > 0) {
		//CHECK_NE(name##_data, nullptr);
	}
	char* const target_data = static_cast<char*>(target->Buffer()->Data()) + target_offset;
	uint32_t target_start;
	uint32_t source_start;
	uint32_t to_copy;
	if(!args[2]->Uint32Value(context).To(&target_start) || !args[3]->Uint32Value(context).To(&source_start) || !args[4]->Uint32Value(context).To(&to_copy)) {
		return;
	}
	memmove(target_data + target_start, source.data() + source_start, to_copy);
	args.GetReturnValue().Set(to_copy);
}
void slim::plugin::node::Buffer::IndexOfBuffer(const v8::FunctionCallbackInfo<v8::Value>& args) {
/* 	CHECK(args[1]->IsObject());
	CHECK(args[2]->IsNumber());
	CHECK(args[3]->IsInt32());
	CHECK(args[4]->IsBoolean());*/
	enum encoding enc = static_cast<enum encoding>(args[3].As<v8::Int32>()->Value());
/* 	THROW_AND_RETURN_UNLESS_BUFFER(Environment::GetCurrent(args), args[0]);
	THROW_AND_RETURN_UNLESS_BUFFER(Environment::GetCurrent(args), args[1]); */
	ArrayBufferViewContents<char> haystack_contents(args[0]);
	ArrayBufferViewContents<char> needle_contents(args[1]);
	int64_t offset_i64 = args[2].As<v8::Integer>()->Value();
	bool is_forward = args[4]->IsTrue();
	const char* haystack = haystack_contents.data();
	const size_t haystack_length = haystack_contents.length();
	const char* needle = needle_contents.data();
	const size_t needle_length = needle_contents.length();
	int64_t opt_offset = IndexOfOffset(haystack_length, offset_i64, needle_length, is_forward);
	if(needle_length == 0) {
	  	// Match String#indexOf() and String#lastIndexOf() behavior.
	  	args.GetReturnValue().Set(static_cast<double>(opt_offset));
	  	return;
	}
	if(haystack_length == 0) {
	  	return args.GetReturnValue().Set(-1);
	}
	if(opt_offset <= -1) {
		return args.GetReturnValue().Set(-1);
	}
	size_t offset = static_cast<size_t>(opt_offset);
	//CHECK_LT(offset, haystack_length);
	if ((is_forward && needle_length + offset > haystack_length) || needle_length > haystack_length) {
		return args.GetReturnValue().Set(-1);
	}
	size_t result = haystack_length;
	if(enc == node::encoding::UCS2) {
		if(haystack_length < 2 || needle_length < 2) {
			return args.GetReturnValue().Set(-1);
	  	}
	  	result = nbytes::SearchString(reinterpret_cast<const uint16_t*>(haystack), haystack_length / 2,
									reinterpret_cast<const uint16_t*>(needle), needle_length / 2, offset / 2, is_forward);
	  	result *= 2;
	}
	else {
		result = nbytes::SearchString(reinterpret_cast<const uint8_t*>(haystack), haystack_length,
									reinterpret_cast<const uint8_t*>(needle), needle_length, offset, is_forward);
	}
	args.GetReturnValue().Set(result == haystack_length ? -1 : static_cast<int>(result));
}
void slim::plugin::node::Buffer::SlowIndexOfNumber(const v8::FunctionCallbackInfo<v8::Value>& args) {
/* 	CHECK(args[1]->IsUint32());
	CHECK(args[2]->IsNumber());
	CHECK(args[3]->IsBoolean());
	THROW_AND_RETURN_UNLESS_BUFFER(Environment::GetCurrent(args), args[0]); */
	ArrayBufferViewContents<uint8_t> buffer(args[0]);
	uint32_t needle = args[1].As<v8::Uint32>()->Value();
	int64_t offset_i64 = args[2].As<v8::Integer>()->Value();
	bool is_forward = args[3]->IsTrue();
	args.GetReturnValue().Set(IndexOfNumber(buffer.data(), buffer.length(), needle, offset_i64, is_forward));
}
bool slim::plugin::node::Buffer::HasInstance(v8::Local<v8::Value> val) {
	return val->IsArrayBufferView();
}
void slim::plugin::node::Buffer::IndexOfString(const v8::FunctionCallbackInfo<v8::Value>& args) {
	auto* isolate = args.GetIsolate();
	//Environment* env = Environment::GetCurrent(args);
	//Isolate* isolate = env->isolate();
/* 	CHECK(args[1]->IsString());
	CHECK(args[2]->IsNumber());
	CHECK(args[3]->IsInt32());
	CHECK(args[4]->IsBoolean()); */
	enum encoding enc = static_cast<enum encoding>(args[3].As<v8::Int32>()->Value());
	//THROW_AND_RETURN_UNLESS_BUFFER(env, args[0]);
	ArrayBufferViewContents<char> buffer(args[0]);
	v8::Local<v8::String> needle = args[1].As<v8::String>();
	int64_t offset_i64 = args[2].As<v8::Integer>()->Value();
	bool is_forward = args[4]->IsTrue();
	const char* haystack = buffer.data();
	// Round down to the nearest multiple of 2 in case of UCS2.
	const size_t haystack_length = (enc == UCS2) ? buffer.length() &~ 1 : buffer.length();  // NOLINT(whitespace/operators)
	size_t needle_length;
	if(!StringBytes::Size(isolate, needle, enc).To(&needle_length)) {
		return;
	}
	int64_t opt_offset = IndexOfOffset(haystack_length, offset_i64, needle_length, is_forward);
	if(needle_length == 0) {
	  	// Match String#indexOf() and String#lastIndexOf() behavior.
	  	args.GetReturnValue().Set(static_cast<double>(opt_offset));
	  	return;
	}
	if(haystack_length == 0) {
	  	return args.GetReturnValue().Set(-1);
	}
	if (opt_offset <= -1) {
	  return args.GetReturnValue().Set(-1);
	}
	size_t offset = static_cast<size_t>(opt_offset);
	//CHECK_LT(offset, haystack_length);
	if ((is_forward && needle_length + offset > haystack_length) || needle_length > haystack_length) {
	  	return args.GetReturnValue().Set(-1);
	}
	size_t result = haystack_length;
	if(enc == UCS2) {
	  	v8::String::Value needle_value(isolate, needle);
	  	if(*needle_value == nullptr) {
			return args.GetReturnValue().Set(-1);
	  	}
	  	if(haystack_length < 2 || needle_value.length() < 1) {
			return args.GetReturnValue().Set(-1);
	  	}
	  	if constexpr (IsBigEndian()) {
			node::StringBytes::StringBytes::InlineDecoder decoder;
			if(decoder.Decode(isolate, needle, enc).IsNothing()) {
				return;
			}
			const uint16_t* decoded_string = reinterpret_cast<const uint16_t*>(decoder.out());
			if(decoded_string == nullptr) {
				return args.GetReturnValue().Set(-1);
			}
			result = nbytes::SearchString(reinterpret_cast<const uint16_t*>(haystack), haystack_length / 2,
									  							decoded_string, decoder.size() / 2, offset / 2, is_forward);
	  	}
	  	else {
			result = nbytes::SearchString(reinterpret_cast<const uint16_t*>(haystack), haystack_length / 2,
								 reinterpret_cast<const uint16_t*>(*needle_value), needle_value.length(), offset / 2, is_forward);
	  	}
	  	result *= 2;
	}
	else if (enc == UTF8) {
	  	v8::String::Utf8Value needle_value(isolate, needle);
	  	if (*needle_value == nullptr) {
			return args.GetReturnValue().Set(-1);
		}
	  	result = nbytes::SearchString(reinterpret_cast<const uint8_t*>(haystack), haystack_length,
							   reinterpret_cast<const uint8_t*>(*needle_value), needle_length, offset, is_forward);
	}
	else if(enc == LATIN1) {
	  	uint8_t* needle_data = node::UncheckedMalloc<uint8_t>(needle_length);
	  	if(needle_data == nullptr) {
			return args.GetReturnValue().Set(-1);
	  	}
	  	needle->WriteOneByte(isolate, needle_data, 0, needle_length, v8::String::NO_NULL_TERMINATION);
	  	result = nbytes::SearchString(reinterpret_cast<const uint8_t*>(haystack), haystack_length, needle_data,
																					needle_length, offset, is_forward);
	  	free(needle_data);
	}
	args.GetReturnValue().Set(result == haystack_length ? -1 : static_cast<int>(result));
}
size_t slim::plugin::node::Buffer::Length(v8::Local<v8::Value> val) {
	//CHECK(val->IsArrayBufferView());
	v8::Local<v8::ArrayBufferView> ui = val.As<v8::ArrayBufferView>();
	return ui->ByteLength();
}
#define SPREAD_BUFFER_ARG(val, name)                                           \
  v8::Local<v8::ArrayBufferView> name = (val).As<v8::ArrayBufferView>();       \
  const size_t name##_offset = name->ByteOffset();                             \
  const size_t name##_length = name->ByteLength();                             \
  char* const name##_data = static_cast<char*>(name->Buffer()->Data()) + name##_offset;  \
  if (name##_length > 0) CHECK_NE(name##_data, nullptr);
void slim::plugin::node::Buffer::Swap16(const v8::FunctionCallbackInfo<v8::Value>& args) {
	//Environment* env = Environment::GetCurrent(args);
	//THROW_AND_RETURN_UNLESS_BUFFER(env, args[0]);
	//SPREAD_BUFFER_ARG(args[0], ts_obj);
	v8::Local<v8::ArrayBufferView> ts_obj = args[0].As<v8::ArrayBufferView>();
	const size_t ts_obj_offset = ts_obj->ByteOffset();
	const size_t ts_obj_length = ts_obj->ByteLength();
	char* const ts_obj_data = static_cast<char*>(ts_obj->Buffer()->Data()) + ts_obj_offset;
	if (ts_obj_length > 0) {
		//CHECK_NE(ts_obj_data, nullptr);
	} 
	if(!nbytes::SwapBytes16(ts_obj_data, ts_obj_length)) {
		//
	}
	args.GetReturnValue().Set(args[0]);
}  
void slim::plugin::node::Buffer::Swap32(const v8::FunctionCallbackInfo<v8::Value>& args) {
	//Environment* env = Environment::GetCurrent(args);
	//THROW_AND_RETURN_UNLESS_BUFFER(env, args[0]);
	//SPREAD_BUFFER_ARG(args[0], ts_obj);
	//CHECK(nbytes::SwapBytes32(ts_obj_data, ts_obj_length));
	v8::Local<v8::ArrayBufferView> ts_obj = args[0].As<v8::ArrayBufferView>();
	const size_t ts_obj_offset = ts_obj->ByteOffset();
	const size_t ts_obj_length = ts_obj->ByteLength();
	char* const ts_obj_data = static_cast<char*>(ts_obj->Buffer()->Data()) + ts_obj_offset;
	if (ts_obj_length > 0) {
		//CHECK_NE(ts_obj_data, nullptr);
	} 
	if(!nbytes::SwapBytes32(ts_obj_data, ts_obj_length)) {
		//
	}
	args.GetReturnValue().Set(args[0]);
}
void slim::plugin::node::Buffer::Swap64(const v8::FunctionCallbackInfo<v8::Value>& args) {
	//Environment* env = Environment::GetCurrent(args);
	//THROW_AND_RETURN_UNLESS_BUFFER(env, args[0]);
	//SPREAD_BUFFER_ARG(args[0], ts_obj);
	//CHECK(nbytes::SwapBytes64(ts_obj_data, ts_obj_length));
	v8::Local<v8::ArrayBufferView> ts_obj = args[0].As<v8::ArrayBufferView>();
	const size_t ts_obj_offset = ts_obj->ByteOffset();
	const size_t ts_obj_length = ts_obj->ByteLength();
	char* const ts_obj_data = static_cast<char*>(ts_obj->Buffer()->Data()) + ts_obj_offset;
	if (ts_obj_length > 0) {
		//CHECK_NE(ts_obj_data, nullptr);
	} 
	if(!nbytes::SwapBytes64(ts_obj_data, ts_obj_length)) {
		//
	}
	args.GetReturnValue().Set(args[0]);
}
v8::Maybe<size_t> slim::plugin::node::StringBytes::Size(v8::Isolate* isolate, v8::Local<v8::Value> val, enum encoding encoding) {
	v8::HandleScope scope(isolate);
	if(Buffer::HasInstance(val) && (encoding == BUFFER || encoding == LATIN1)) {
		return v8::Just(Buffer::Length(val));
	}
	v8::Local<v8::String> str;
	if(!val->ToString(isolate->GetCurrentContext()).ToLocal(&str)) {
		return v8::Nothing<size_t>();
	}
	v8::String::ValueView view(isolate, str);
	switch(encoding) {
		case ASCII:
		case LATIN1:
			return v8::Just<size_t>(view.length());
		case BUFFER:
		case UTF8:
			if(view.is_one_byte()) {
				return v8::Just<size_t>(simdutf::utf8_length_from_latin1(reinterpret_cast<const char*>(view.data8()), view.length()));
			}
			return v8::Just<size_t>(simdutf::utf8_length_from_utf16(reinterpret_cast<const char16_t*>(view.data16()), view.length()));
		case UCS2:
			return v8::Just(view.length() * sizeof(uint16_t));
		case BASE64URL: {
				return v8::Just(simdutf::base64_length_from_binary(view.length(), simdutf::base64_url));
			}
		case BASE64: {
			return v8::Just(simdutf::base64_length_from_binary(view.length()));
		}
		case HEX:
			return v8::Just<size_t>(view.length() / 2);
	}
}
static void slim::plugin::node::internal::util::GetOwnNonIndexProperties(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Local<v8::Context> context = args.GetIsolate()->GetCurrentContext();
/* 	CHECK(args[0]->IsObject());
	CHECK(args[1]->IsUint32()); */
	v8::Local<v8::Object> object = args[0].As<v8::Object>();
	v8::Local<v8::Array> properties;
	v8::PropertyFilter filter = static_cast<v8::PropertyFilter>(args[1].As<v8::Uint32>()->Value());
	if(!object->GetPropertyNames(context, v8::KeyCollectionMode::kOwnOnly, filter, v8::IndexFilter::kSkipIndices).ToLocal(&properties)) {
		return;
	}
	args.GetReturnValue().Set(properties);
  }
static void slim::plugin::node::internal::util::IsInsideNodeModules(const v8::FunctionCallbackInfo<v8::Value>& args) {
	auto* isolate = args.GetIsolate();
	//CHECK_EQ(args.Length(), 2);
	//CHECK(args[0]->IsInt32());  // frame_limit
	// The second argument is the default value.
	int frames_limit = args[0].As<v8::Int32>()->Value();
	v8::Local<v8::StackTrace> stack = v8::StackTrace::CurrentStackTrace(isolate, frames_limit);
	int frame_count = stack->GetFrameCount();
	// If the search requires looking into more than |frames_limit| frames, give
	// up and return the specified default value.
	if(frame_count == frames_limit) {
		return args.GetReturnValue().Set(args[1]);
	}
	bool result = false;
	for(int i = 0; i < frame_count; ++i) {
	  	v8::Local<v8::StackFrame> stack_frame = stack->GetFrame(isolate, i);
	  	v8::Local<v8::String> script_name = stack_frame->GetScriptName();
	  	if(script_name.IsEmpty() || script_name->Length() == 0) {
			continue;
	  	}
	  	auto script_name_str = slim::utilities::v8StringToString(isolate, script_name);
	  	if(script_name_str.starts_with("node:")) {
			continue;
	  	}
	  	if(script_name_str.find("/node_modules/") != std::string::npos || script_name_str.find("\\node_modules\\") != std::string::npos ||
					script_name_str.find("/node_modules\\") != std::string::npos || script_name_str.find("\\node_modules/") != std::string::npos) {
			result = true;
			break;
	  	}
	}
	args.GetReturnValue().Set(result);
}