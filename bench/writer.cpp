#include <bench/writer.hpp>

namespace bench {


static WriterVTable buffer_writer_vtable = {
	[](void* userdata, const void* buffer, I32 size) {
		BufferWriter* writer = (BufferWriter*)userdata;
		memcpy(writer->head, buffer, size);
		writer->head += size;
	},
};

BufferWriter::BufferWriter(U8* buffer, I32 size) {
    m_buffer = buffer;
    m_head = buffer;
    m_end = buffer + size;
}

BufferWriter::operator Writer() const {
	return Writer{ &buffer_writer_vtable, (void*)this };
}

}