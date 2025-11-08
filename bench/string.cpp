#include <bench/string.hpp>
#include <bench/writer.hpp>
#include <stdlib.h>

namespace bench {

bool String::operator==(String other) const {
	if (this->length != other.length)
		return false;

	return memcmp(this->data, other.data, this->length) == 0;
}

bool String::operator ==(const char* other) const {
	U32 other_length = strlen(other);
	if (this->length != other_length)
		return false;

	return memcmp(this->data, other, this->length) == 0;
}

bool String::Cut(U8 byte, String& a, String& b) const {
	for (U32 i = 0; i < length; i++) {
		if (data[i] == byte) {
			a = String(data, i);
			b = String(data + i + 1, length - i - 1);
			return true;
		}
	}
	a = *this;
	b = {};
	return false;
}

String String::Trim() const {
	U32 start;
	for (start = 0; start < length; start++) {
		if (data[start] != ' ' && data[start] != '\n' && data[start] != '\t')
			break;
	}

	U32 end;
	for (end = length - 1; end > 0; end--) { // >0 because it's unsigned!
		if (data[end] != ' ' && data[end] != '\n' && data[end] != '\t')
			break;
	}

	if (end < start)
		return {};
	else
		return String(data + start, end - start + 1);
}

String String::CopyToHeap() const {
	U8* copy_data = (U8*)malloc(length);
	memcpy(copy_data, data, length);
	return String(copy_data, length);
}

void String::FreeFromHeap() {
	free(data);
	*this = {};
}

}
