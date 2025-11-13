#include <bench/core/string.hpp>
#include <bench/core/writer.hpp>
#include <stdlib.h>
#include <string.h>

namespace bench {


String::String(const char* cstring) {
	assert(cstring);
	this->data = (U8*)cstring;
	this->length = strlen(cstring);
}

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

static String CopyStringToHeap(String string) {
	U8* copy_data = (U8*)malloc(string.length);
	memcpy(copy_data, string.data, string.length);
	return String(copy_data, string.length);
}

HeapString::HeapString() {
	m_string = {};
}

HeapString::HeapString(String string) {
	m_string = CopyStringToHeap(string);
}

HeapString::HeapString(const char* cstring) {
	m_string = CopyStringToHeap(String(cstring));
}

HeapString::HeapString(const HeapString& other) {
	m_string = CopyStringToHeap(other.m_string);
}

HeapString::HeapString(HeapString&& other) {
	m_string = other.m_string;
	other.m_string = {};
}

HeapString& HeapString::operator=(const HeapString& other) {
	if (this != &other) {
		if (m_string.data)
			free(m_string.data);
		m_string = CopyStringToHeap(other.m_string);
	}
	return *this;
}

HeapString& HeapString::operator=(HeapString&& other) {
	if (this != &other) {
		if (m_string.data)
			free(m_string.data);
		m_string = other.m_string;
		other.m_string = {};
	}
	return *this;
}

HeapString::~HeapString() {
	if (m_string.data)
		free(m_string.data);
}

HeapString::operator String() const {
	return m_string;
}

bool HeapString::operator==(String other) const {
	return m_string == other;
}

bool HeapString::operator==(const char* cstring) const {
	return m_string == cstring;
}


}
