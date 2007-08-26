
#include "xtal.h"
#include "xtal_stream.h"
#include "xtal_serializer.h"
#include "xtal_macro.h"

namespace xtal{

void InitStream(){

	{
		ClassPtr p = new_cpp_class<Stream>("Stream");
		p->method("put_i8", &Stream::put_i8);
		p->method("put_i16", &Stream::put_i16);
		p->method("put_i32", &Stream::put_i32);
		p->method("put_u8", &Stream::put_u8);
		p->method("put_u16", &Stream::put_u16);
		p->method("put_u32", &Stream::put_u32);
		p->method("get_i8", &Stream::get_i8);
		p->method("get_i16", &Stream::get_i16);
		p->method("get_i32", &Stream::get_i32);
		p->method("get_u8", &Stream::get_u8);
		p->method("get_u16", &Stream::get_u16);
		p->method("get_u32", &Stream::get_u32);
		p->method("put_f32", &Stream::put_f32);
		p->method("get_f32", &Stream::get_f32);
		
		p->method("get_s", &Stream::get_s)->param(Named("length", -1));
		p->method("put_s", &Stream::put_s);

		p->method("print", &Stream::print);
		p->method("println", &Stream::println);

		p->method("seek", &Stream::seek)->param(null, Named("whence", Stream::XSEEK_SET));
		p->method("tell", &Stream::tell);
		p->method("pour", &Stream::pour);
		p->method("pour_all", &Stream::pour_all);
		p->method("size", &Stream::size);

		p->method("serialize", &Stream::serialize);
		p->method("deserialize", &Stream::deserialize);
		p->method("xtalize", &Stream::xtalize);
		p->method("dextalize", &Stream::dextalize);

		p->method("iter_first", &Stream::iter_first);
		p->method("iter_next", &Stream::iter_next);
		p->method("iter_break", &Stream::iter_break);

		p->def("SEEK_SET", Stream::XSEEK_SET);
		p->def("SEEK_CUR", Stream::XSEEK_CUR);
		p->def("SEEK_END", Stream::XSEEK_END);
	}

	{
		ClassPtr p = new_cpp_class<MemoryStream>("MemoryStream");
		p->inherit(get_cpp_class<Stream>());
		p->def("new", ctor<MemoryStream>());
		p->method("to_s", &MemoryStream::to_s);
		p->method("resize", &MemoryStream::resize);
	}

	{
		ClassPtr p = new_cpp_class<StringStream>("StringStream");
		p->inherit(get_cpp_class<Stream>());
		p->def("new", ctor<StringStream, const StringPtr&>());
	}

	{
		ClassPtr p = new_cpp_class<FileStream>("FileStream");
		p->inherit(get_cpp_class<Stream>());
		p->def("new", ctor<FileStream, const StringPtr&, const StringPtr&>()->param(Named("name"), Named("mode", "r")));
	}

}


void Stream::put_s(const StringPtr& str){
	write(str->c_str(), str->buffer_size());
}

StringPtr Stream::get_s(int_t length){
	if(eof())
		return "";

	if(length==1){
		char_t buf[16];
		buf[0] = get_ch();

		int_t len = ch_len(buf[0]);
		if(len<0){
			for(int_t i=1; i<-len; ++i){
				buf[i] = get_ch();
			}
			int_t len2 = ch_len2(buf);
			for(int_t i=len; i<len2; ++i){
				buf[i] = get_ch();
			}
		}else{
			for(int_t i=1; i<len; ++i){
				buf[i] = get_ch();
			}
		}
		return xnew<String>(buf, len);
	}

	if(length<0){
		MemoryStreamPtr ms = xnew<MemoryStream>();
		while(!eof()){
			ms->put_s(get_s(1));
		}
		return ms->to_s();
	}

	MemoryStreamPtr ms = xnew<MemoryStream>();
	for(int_t i=0; i<length; ++i){
		ms->put_s(get_s(1));
	}
	return ms->to_s();
}

uint_t Stream::print(const StringPtr& str){
	return write(str->c_str(), str->buffer_size());
}

void Stream::println(const StringPtr& str){
	write(str->c_str(), str->buffer_size());
	write("\n", 1);
}

uint_t Stream::pour(const StreamPtr& in_stream, uint_t size){
	xtal::u8* buf = (xtal::u8*)user_malloc(size*sizeof(xtal::u8));
	uint_t len = in_stream->read(buf, size);
	write(buf, len);
	user_free(buf);
	return len;
}

uint_t Stream::pour_all(const StreamPtr& in_stream){
	uint_t size = 1024*10, len, sum = 0;
	xtal::u8* buf = (xtal::u8*)user_malloc(size*sizeof(xtal::u8));
	do{
		len = in_stream->read(buf, size);
		sum += len;
		write(buf, len);
	}while(len==size);
	user_free(buf);
	return sum;
}

uint_t Stream::size(){
	uint_t pos = tell();
	seek(0, XSEEK_END);
	uint_t len = tell();
	seek(pos, XSEEK_SET);
	return len;
}

void Stream::iter_first(const VMachinePtr& vm){
	vm->return_result(StreamPtr::from_this(this), StreamPtr::from_this(this));
}

void Stream::iter_next(const VMachinePtr& vm){
	vm->return_result(null, null);
	close();
}

void Stream::iter_break(const VMachinePtr& vm){
	close();
}

void Stream::serialize(const AnyPtr& obj){
	Serializer s(StreamPtr::from_this(this));
	s.serialize(obj);
}

AnyPtr Stream::deserialize(){
	Serializer s(StreamPtr::from_this(this));
	return s.deserialize();
}

void Stream::xtalize(const AnyPtr& obj){

}

AnyPtr Stream::dextalize(){
	return null;
}


FileStream::FileStream(const StringPtr& filename, const StringPtr& mode){
	const char_t* str = mode->c_str();
	while(*str){
		if(*str=='t'){
			break;
		}
		str++;
	}

	StringPtr bmode = *str=='t' ? mode : mode->cat("b");
	fp_ = fopen(filename->c_str(), bmode->c_str());
	if(!fp_){
		full_gc();
		fp_ = fopen(filename->c_str(), bmode->c_str());
		if(!fp_){
			XTAL_THROW(builtin()->member(Xid(IOError))(Xt("Xtal Runtime Error 1014")(Named("name", filename))), return);
		}
	}
}

FileStream::FileStream(FILE* fp){
	fp_ = fp;
}

FileStream::~FileStream(){
	close();
}

uint_t FileStream::tell(){
	if(!fp_){ XTAL_THROW(builtin()->member(Xid(IOError))(Xt("Xtal Runtime Error 1018")), return 0); }

	return ftell(fp_);
}

uint_t FileStream::write(const void* p, uint_t size){
	if(!fp_){ XTAL_THROW(builtin()->member(Xid(IOError))(Xt("Xtal Runtime Error 1018")), return 0); }

	XTAL_UNLOCK{
		return fwrite(p, 1, size, fp_);
	}
	return 0;
}

uint_t FileStream::read(void* p, uint_t size){
	if(!fp_){ XTAL_THROW(builtin()->member(Xid(IOError))(Xt("Xtal Runtime Error 1018")), return 0); }

	XTAL_UNLOCK{
		return fread(p, 1, size, fp_);
	}
	return 0;
}

void FileStream::seek(int_t offset, int_t whence){
	if(!fp_){ XTAL_THROW(builtin()->member(Xid(IOError))(Xt("Xtal Runtime Error 1018")), return); }

	int wh = whence==XSEEK_END ? SEEK_END : whence==XSEEK_CUR ? SEEK_CUR : SEEK_SET;
	fseek(fp_, offset, wh);
}

void FileStream::close(){
	if(fp_){
		fclose(fp_);
		fp_ = 0;
	}
}

bool FileStream::eof(){
	if(!fp_){ return true; }
	return feof(fp_)!=0;
}

MemoryStream::MemoryStream(){
	pos_ = 0;
}

MemoryStream::MemoryStream(const void* data, uint_t data_size){
	data_.resize(data_size);
	if(data_size>0){
		memcpy(&data_[0], data, data_size);
	}
	pos_ = 0;
}

uint_t MemoryStream::tell(){
	return pos_;
}

uint_t MemoryStream::write(const void* p, uint_t size){
	if(pos_+size>data_.size()){ 
		data_.resize(pos_+size);
	}
	if(size>0){
		memcpy(&data_[pos_], p, size);
	}
	pos_ += size;
	return size;	
}

uint_t MemoryStream::read(void* p, uint_t size){
	if(pos_+size>data_.size()){ 
		uint_t diff = data_.size()-pos_;
		if(diff>0){
			memcpy(p, &data_[pos_], diff);
		}
		pos_ += diff;
		return diff; 
	}
	
	if(size>0){
		memcpy(p, &data_[pos_], size);
	}
	pos_ += size;
	return size;
}

void MemoryStream::seek(int_t offset, int_t whence){
	switch(whence){
		case XSEEK_END:
			pos_ = data_.size()-offset;
			break;
		case XSEEK_CUR:
			pos_ += offset;
			break;
		default:
			if(offset<0){
				offset = 0;
			}
			pos_ = offset;
			break;
	}
}

uint_t MemoryStream::pour(const StreamPtr& in_stream, uint_t size){
	if(size==0){
		return 0;
	}

	if(data_.size() <= pos_ + size){
		data_.resize(pos_+size);
	}

	uint_t len = in_stream->read(&data_[pos_], size);
	data_.resize(data_.size() - (size - len));
	pos_ += len;
	return len;
}

uint_t MemoryStream::pour_all(const StreamPtr& in_stream){
	uint_t size = 1024*10, len, sum = 0;
	do{
		if(data_.size() <= pos_ + size){
			data_.resize(pos_+size);
		}

		len = in_stream->read(&data_[pos_], size);
		sum += len;
	}while(len==size);
	data_.resize(data_.size() - (size - len));
	pos_ += sum;
	return sum;
}

StringPtr MemoryStream::get_s(int_t length){
	if(pos_ >= data_.size())
		return "";

	if(length==1){
		uint_t pos = pos_;

		int_t len = ch_len(data_[pos_]);
		if(len<0){
			if(pos_ + -len > data_.size()){
				return "";
			}
			len = ch_len2((char_t*)&data_[pos_]);
		}

		if(pos_ + len > data_.size()){
			return "";
		}

		pos_ += len;

		switch(len){
		case 1: return xnew<String>(data_[pos]);
		case 2: return xnew<String>(data_[pos], data_[pos+1]);
		case 3: return xnew<String>(data_[pos], data_[pos+1], data_[pos+2]);
		default: return xnew<String>((char_t*)&data_[pos], len);
		}
	}
	
	if(length<0){
		StringPtr ret = xnew<String>((char_t*)&data_[pos_], data_.size() - pos_);
		pos_ = data_.size();
		return ret;
	}

	int_t slen = 0;
	int_t blen = 0;
	while(slen<length){
		if(pos_ + blen >= data_.size()){
			break;
		}

		int_t len = ch_len(data_[pos_ + blen]);
		if(len<0){
			if(pos_ + blen + -len > data_.size()){
				break;
			}
			len = ch_len2((char_t*)&data_[pos_ + blen]);
		}

		if(pos_ + blen + len > data_.size()){
			break;
		}

		blen += len;
		slen++;
	}

	if(blen==0)
		return "";

	StringPtr ret = xnew<String>((char_t*)&data_[pos_], blen);
	pos_ += blen;
	return ret;	
}

StringPtr MemoryStream::to_s(){
	if(data_.empty())
		return "";
	return xnew<String>((char_t*)&data_[0], data_.size());
}

bool MemoryStream::eof(){
	return pos_>=data_.size();
}


StringStream::StringStream(const StringPtr& str)
:str_(str ? str : StringPtr("")){
	data_ = str_->c_str();
	size_ = str_->buffer_size();
	pos_ = 0;
}
	
uint_t StringStream::tell(){
	return pos_;
}

uint_t StringStream::write(const void* p, uint_t size){
	XTAL_THROW(unsupported_error("StringStream", "write"), return 0);
}

uint_t StringStream::read(void* p, uint_t size){
	if(pos_+size>size_){ 
		uint_t diff = size_-pos_;
		if(diff>0){
			memcpy(p, &data_[pos_], diff);
		}
		pos_ += diff;
		return diff; 
	}
	
	if(size>0){
		memcpy(p, &data_[pos_], size);
	}
	pos_ += size;
	return size;
}

void StringStream::seek(int_t offset, int_t whence){
	switch(whence){
		case XSEEK_END:
			pos_ = size_-offset;
			break;
		case XSEEK_CUR:
			pos_ += offset;
			break;
		default:
			if(offset<0){
				offset = 0;
			}
			pos_ = offset;
			break;
	}
}

StringPtr StringStream::get_s(int_t length){
	if(pos_ >= size_)
		return "";

	if(length==1){
		uint_t pos = pos_;
		int_t len = ch_len(data_[pos_]);
		if(len<0){
			if(pos_ + -len > size_){
				return "";
			}
			len = ch_len2(&data_[pos_]);
		}

		if(pos_ + len > size_){
			return "";
		}

		pos_ += len;

		switch(len){
		case 1: return xnew<String>(data_[pos]);
		case 2: return xnew<String>(data_[pos], data_[pos+1]);
		case 3: return xnew<String>(data_[pos], data_[pos+1], data_[pos+2]);
		default: return xnew<String>((char_t*)&data_[pos], len);
		}
	}

	if(length<0){
		StringPtr ret = xnew<String>((char_t*)&data_[pos_], size_ - pos_);
		pos_ = size_;
		return ret;
	}

	int_t slen = 0;
	int_t blen = 0;
	while(slen<length){
		if(pos_ + blen >= size_){
			break;
		}

		int_t len = ch_len(data_[pos_ + blen]);
		if(len<0){
			if(pos_ + -len > size_){
				break;
			}
			len = ch_len2(&data_[pos_ + blen]);
		}

		if(pos_ + blen + len > size_){
			break;
		}

		blen += len;
		slen++;
	}

	if(blen==0)
		return "";

	StringPtr ret = xnew<String>((char_t*)&data_[pos_], blen);
	pos_ += blen;
	return ret;	
}

bool StringStream::eof(){
	return pos_>=size_;
}

InteractiveStream::InteractiveStream(){
	line_ = 1;
	continue_stmt_ = false;
	fp_ = stdin;
}

uint_t InteractiveStream::tell(){
	XTAL_THROW(unsupported_error("InteractiveStream", "tell"), return 0);
}

uint_t InteractiveStream::write(const void* p, uint_t size){
	XTAL_THROW(unsupported_error("InteractiveStream", "write"), return 0);
}

uint_t InteractiveStream::read(void* p, uint_t size){
	if(!fp_)
		return 0;
	if(continue_stmt_){
		fprintf(stdout, "ix:%03d>    ", line_);
	}else{
		fprintf(stdout, "ix:%03d>", line_);
	}
	continue_stmt_ = true;
	if(fgets((char*)p, size, fp_)){
		uint_t sz = strlen((char*)p);
		if(sz!=size-1){
			line_++;
		}
		return sz;
	}
	fp_ = 0;
	return 0;
}

void InteractiveStream::seek(int_t offset, int_t whence){
	XTAL_THROW(unsupported_error("InteractiveStream", "seek"), return);
}

void InteractiveStream::close(){
	if(fp_){
		fclose(fp_);
		fp_ = 0;
	}
}

void InteractiveStream::set_continue_stmt(bool b){
	continue_stmt_ = b;
}


}
