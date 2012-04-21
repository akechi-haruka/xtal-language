/** \file src/xtal/xtal_map.h
* \brief src/xtal/xtal_map.h
*/

#ifndef XTAL_MAP_H_INCLUDE_GUARD
#define XTAL_MAP_H_INCLUDE_GUARD

#pragma once

namespace xtal{

/**
* \xbind lib::builtin
* \xinherit lib::builtin::Any lib::builtin::Iterable
* \brief �A�z�z��
*/
class Map : public Base{
public:

	/**
	* \xbind
	* \brief ��̘A�z�z��I�u�W�F�N�g�𐶐�
	*/
	Map();

	~Map();
	
	/**
	* \brief key�ɑΉ�����v�f��Ԃ�
	* \param key �����̂��߂̃L�[ 
	*/
	const AnyPtr& at(const AnyPtr& key);
	
	/**
	* \brief key�ɑΉ�����v�f��ݒ肷��
	* \param key �����̂��߂̃L�[ 
	* \param value key�ɑΉ�����l
	*/	
	void set_at(const AnyPtr& key, const AnyPtr& value){
		insert(key, value);
	}

	/**
	* \xbind
	* \brief key�ɑΉ�����v�f��Ԃ�
	* \param key �����̂��߂̃L�[ 
	*/
	const AnyPtr& op_at(const AnyPtr& key);
	
	/**
	* \xbind
	* \brief key�ɑΉ�����v�f��ݒ肷��
	* \param key �����̂��߂̃L�[ 
	* \param value key�ɑΉ�����l
	*/	
	void op_set_at(const AnyPtr& key, const AnyPtr& value){
		insert(key, value);
	}

	/**
	* \xbind
	* \brief key�ɑΉ�����v�f��ݒ肷��
	* \param key �����̂��߂̃L�[ 
	* \param value key�ɑΉ�����l
	*/	
	void insert(const AnyPtr& key, const AnyPtr& value);

	/**
	* \xbind
	* \brief key�ɑΉ�����l���폜����
	* \param key �����̂��߂̃L�[ 
	*/
	void erase(const AnyPtr& key);

	/**
	* \xbind
	* \brief �A�z�z��ɓo�^���ꂽ�v�f�̐���Ԃ�
	* \return �A�z�z��ɓo�^���ꂽ�v�f�̐�
	*/
	uint_t size(){
		return table_.size();
	}
	
	/**
	* \xbind
	* \brief �A�z�z��ɓo�^���ꂽ�v�f�̐���Ԃ�
	* \return �A�z�z��ɓo�^���ꂽ�v�f�̐�
	*/
	uint_t length(){
		return table_.size();
	}

	/**
	* \xbind
	* \brief �󂩒��ׂ�
	* \retval true ��
	* \retval false ���
	*/
	bool empty(){
		return table_.empty();
	}

	/**
	* \xbind
	* \brief �󂩒��ׂ�
	* \retval true ��
	* \retval false ���
	*/
	bool is_empty(){
		return table_.empty();
	}

	/**
	* \xbind
	* \brief �A�������A�z�z���Ԃ�
	*/
	MapPtr op_cat(const MapPtr& a);

	/**
	* \xbind
	* \brief ���g�ƘA�����A���g��Ԃ�
	*/
	const MapPtr& op_cat_assign(const MapPtr& a);
		
	/**
	* \xbind
	* \brief (key, value)��v�f�Ƃ���AIterator��Ԃ�
	*/
	AnyPtr pairs();
	
	/**
	* \xbind
	* \brief key��v�f�Ƃ���AIterator��Ԃ�
	*/
	AnyPtr keys();
	
	/**
	* \xbind
	* \brief value��v�f�Ƃ���AIterator��Ԃ�
	*/
	AnyPtr values();

	/**
	* \xbind
	* \brief (key, value)��v�f�Ƃ���AIterator��Ԃ�
	* pairs�Ɠ���
	*/
	AnyPtr each(){
		return pairs();
	}
	
	/**
	* \xbind
	* \brief �󂢃R�s�[��Ԃ�
	*/
	MapPtr clone();

	/**
	* \xbind
	* \brief �v�f��S�č폜����
	*/
	void clear();

	/**
	* \xbind
	* \brief ���g��Ԃ�
	*/
	const MapPtr& op_to_map(){
		return to_smartptr(this);
	}

	/**
	* \xbind
	* \brief ���g��iterator�Ŏ擾�ł���v�f�ɒu��������
	*/
	void assign(const AnyPtr& iterator);

	/**
	* \xbind
	* \brief iterator�Ŏ擾�ł���v�f��ǉ�����
	*/
	void append(const AnyPtr& iterator);

	void push_all(const VMachinePtr& vm);

	/**
	* \xbind
	* \brief �f�t�H���g�l�Ƃ��Đݒ肳��Ă���I�u�W�F�N�g�����o��
	* \return �f�t�H���g�l
	*/
	const AnyPtr& default_value(){
		return default_value_;
	}

	/**
	* \xbind
	* \brief �f�t�H���g�l��ݒ肷��
	* \param default_value �f�t�H���g�l
	*/
	void set_default_value(const AnyPtr& default_value){
		default_value_ = default_value;
	}

	StringPtr to_s();

public:

	void block_first(const VMachinePtr& vm);

	struct Fun{
		static uint_t hash(const AnyPtr& key){
			return XTAL_detail_rawhash(key);
		}

		static uint_t eq(const AnyPtr& a, const AnyPtr& b){
			return XTAL_detail_raweq(a, b);
		}
	};

	typedef OrderedHashtable<AnyPtr, AnyPtr, Fun> table_t;

	 AnyPtr calc_key(const AnyPtr& key);

public:

	typedef table_t::iterator iterator;

	iterator begin(){
		return table_.begin();
	}

	iterator end(){
		return table_.end();
	}

	iterator find_direct(const AnyPtr& key){
		return table_.find(key);
	}

	void insert_direct(const AnyPtr& key, const AnyPtr& value){
		table_.insert(key, value);
	}

	void on_visit_members(Visitor& m);
	
protected:
	table_t table_;
	AnyPtr default_value_;

};

class MapIter : public Base{
public:

	MapIter(const MapPtr& m, int_t type);
	
	void block_next(const VMachinePtr& vm);

public:

	bool block_next_direct(AnyPtr& rkey, AnyPtr& rval);

	void on_visit_members(Visitor& m);

private:
	MapPtr map_;
	Map::iterator node_;
	int_t type_;

};

class Set : public Map{
public:

	/**
	* \brief value���^��key��v�f�Ƃ���Iterator��Ԃ�
	*
	*/
	AnyPtr each();

	StringPtr to_s();
};


template<>
struct XNew<Map> : public XXNew<Map>{
	XNew();
};


}//namespace


#endif // XTAL_MAP_H_INCLUDE_GUARD
