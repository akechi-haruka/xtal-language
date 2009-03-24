#ifndef XTAL_SERIALIZER_H_INCLUDE_GUARD
#define XTAL_SERIALIZER_H_INCLUDE_GUARD

#pragma once

namespace xtal{

class Serializer{
public:

	Serializer(const StreamPtr& s);

	void serialize(const AnyPtr& v);

	AnyPtr deserialize();

private:

	void inner_serialize(const AnyPtr& v);
	AnyPtr inner_deserialize();
	
	AnyPtr demangle(const AnyPtr& n);

	int_t register_value(const AnyPtr& v);
	int_t append_value(const AnyPtr& v);

	void put_tab(int_t tab);

	void clear(){
		values_ = xnew<Array>();
		map_ = xnew<Map>();
	}

private:

	enum{ 
		SERIAL_NEW,
		NAME,
		FILELOCAL,
		REF,
		
		TNULL = 32, 
		TUNDEFINED, 
		TFALSE, 
		TTRUE, 
		TINT, 
		TFLOAT, 
		TSTRING,
		TID,
		TARRAY, 
		TMULTI_VALUE,
		TMAP 
	};

private:

	MapPtr map_;
	ArrayPtr values_;
	StreamPtr stream_;

	XTAL_DISALLOW_COPY_AND_ASSIGN(Serializer);
};

}

#endif // XTAL_SERIALIZER_H_INCLUDE_GUARD
