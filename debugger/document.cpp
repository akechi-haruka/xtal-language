#include "document.h"
#include <QtXml>
#include <QDomDocument>

void Document::init(){
	files_.clear();
}

bool Document::save(const QString& filename){
	QFile file(filename);
	if(!file.open(QIODevice::WriteOnly)){
		 return false;
	}
	QTextStream ds(&file);

	QDomDocument doc;

	QDomElement root = doc.createElement("project");
	doc.appendChild(root);

	QDomElement files = doc.createElement("source_path_list");
	root.appendChild(files);

	for(int i=0; i<files_.size(); ++i){
		QDomElement node1 = doc.createElement("path");
		QDomText node2 = doc.createTextNode(files_[i].path);
		node1.appendChild(node2);
		files.appendChild(node1);
	}

	QDomElement eval_exprs = doc.createElement("eval_expr_list");
	root.appendChild(eval_exprs);

	for(int i=0; i<eval_exprs_.size(); ++i){
		QDomElement node1 = doc.createElement("eval_expr");
		QDomText node2 = doc.createTextNode(eval_exprs_[i]);
		node1.appendChild(node2);
		eval_exprs.appendChild(node1);
	}

	doc.save(ds, 4);

	return true;
}

bool Document::load(const QString& filename){
	QFile file(filename);
	if(!file.open(QIODevice::ReadOnly)){
		 return false;
	}

	init();

	QDomDocument doc;
	QString error_message;
	int error_line;
	int error_column;
	if(!doc.setContent(&file, true, &error_message, &error_line, &error_column)){
		return false;
	}

	QDomElement root = doc.documentElement();
	if(root.tagName()!="project"){
		return false;
	}

	for(QDomNode node=root.firstChild(); !node.isNull(); node=node.nextSibling()){
		if(node.toElement().tagName()=="source_path_list"){
			for(QDomNode node2 = node.toElement().firstChild(); !node2.isNull(); node2=node2.nextSibling()){
				QDomNode node3 = node2.toElement().firstChild();
				if(node3.nodeType()==QDomNode::TextNode){
					FileInfo fi;
					fi.path = node3.toText().data();
					files_.push_back(fi);
				}
			}
		}

		if(node.toElement().tagName()=="eval_expr_list"){
			for(QDomNode node2 = node.toElement().firstChild(); !node2.isNull(); node2=node2.nextSibling()){
				QDomNode node3 = node2.toElement().firstChild();
				if(node3.nodeType()==QDomNode::TextNode){
					eval_exprs_.push_back(node3.toText().data());
				}
			}
		}
	}

	return true;
}

Document::FileInfo* Document::file(int i){
	if(i<0 || i>=files_.size()){
		return 0;
	}
	return &files_[i];
}

int Document::file_count(){
	return files_.size();
}

bool Document::add_file(const QString& file){
	if(find_file(file)<0){
		FileInfo fi;
		fi.path = file;
		files_.push_back(fi);
		return true;
	}
	return false;
}

int Document::find_file(const QString& file){
	for(int i=0; i<files_.size(); ++i){
		if(files_[i].path==file){
			return i;
		}
	}
	return -1;
}

int Document::find_file_about(const QString& file){
	QString str = file + ".xtal";
	QRegExp r2(".*\\/(.+)$");
	r2.setMinimal(true);

	while(true){
		QRegExp r(QRegExp::escape(str));

		for(int i=0; i<files_.size(); ++i){
			FileInfo& f = files_[i];

			if(r.indexIn(f.path)>-1){
				return i;
			}
		}

		if(r2.indexIn(str)>-1){
			str = r2.cap(1);
		}
		else{
			break;
		}
	}

	return -1;
}
