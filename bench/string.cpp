#include <bench/string.hpp>

namespace bench {

bool String::Cut(U8 byte, String& a, String& b) {
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

String String::Trim() {
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

	if (end <= start)
		return {};
	else
		return String(data + start, end - start + 1);
}

std::string String::to_std_string() {
	return std::string((const char*)this->data, length);
}

}
