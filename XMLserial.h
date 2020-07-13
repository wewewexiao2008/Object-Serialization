#pragma once
#include<utility>
#include<vector>
#include "tinyxml2.h"
#include<string>
#include<list>
#include<map>
#include<set>
#include<concepts>
using namespace std;
using namespace std;
unsigned int mask6 = (1 << 6) - 1;
unsigned int mask8 = (1 << 8) - 1;

char Base64Alphabet(int x)
{
	if (x < 0 || x>64) throw "Error";
	if (x < 26) return x + 'A';
	else if (x < 52) return x - 26 + 'a';
	else if (x < 62) return x - 52 + '0';
	else if (x == 62) return '+';
	else return '/';
}

int Base64ToInt(char x)
{
	if (x >= 'A' && x <= 'Z') return x - 'A';
	else if (x >= 'a' && x <= 'z') return x - 'a' + 26;
	else if (x >= '0' && x <= '9') return x - '0' + 52;
	else if (x == '+') return 62;
	else if (x == '/') return 63;
	else if (x == '=') return 0;
	else throw "Error";
}

char* StrToBase64(const char* s)
{
	int length = strlen(s);
	char* result = (char*)malloc((length + 2) / 3 * 4 + 1);
	int endpos = 0;
	int cnt = 0;
	for (int i = 0; i < length; i += 3) {
		unsigned int temp = 0;
		for (int j = 0; j < 3; ++j) {
			if (i + j < length) temp |= s[i + j] << ((2 - j) * 8);
			else { endpos = j; break; }
		}
		for (int j = 0; j < 4; ++j)
			result[cnt + j] = Base64Alphabet((temp >> ((3 - j) * 6)) & mask6);
		switch (endpos) {
		case 1: result[cnt + 2] = '=';
		case 2: result[cnt + 3] = '=';
		case 0: break;
		}
		cnt += 4;
	}
	result[cnt] = 0;
	return result;
}

char* Base64ToStr(const char* s)
{
	int length = strlen(s);
	char* result = (char*)malloc(length / 4 * 3 + 1);
	int cnt = 0;
	for (int i = 0; i < length; i += 4) {
		unsigned int temp = 0;
		for (int j = 0; j < 4; ++j)
			temp |= (Base64ToInt(s[i + j])) << ((3 - j) * 6);
		for (int j = 0; j < 3; ++j)
			result[cnt + j] = (temp >> ((2 - j) * 8)) & mask8;
		cnt += 3;
	}
	result[cnt] = 0;
	return result;
}

template<class T>
concept non_string = std::is_arithmetic<T>::value;

// User Defined Type given by example
/*
struct UserDefinedType {
	int idx;
	std::string name;
	std::vector<double> data;
};
*/
// Binary mode switch
static bool is_bin = false;

//vector <UserDefinedType> vectorUserDefinedType;


namespace xml_serialization {
	// potential exception tool
	class serial_error : public exception {
	private:
		string message;
	public:
		serial_error();
		serial_error(string s) :message(s) {}
		string what() { return message; }
	};
	serial_error file_cant_open("Can't Open the File");
	serial_error no_template_in_data("No Particular Template for In Data");
	serial_error no_template_out_data("No Particular Template for Out Data");
	
	/// <summary>
	/// Create a XML node from the given object and XML document existing.
	/// </summary>
	/// <param name="object"> The object to be serialized(may include sub-object) </param>
	/// <param name="doc"> XML document to create new node </param>
	/// <returns></returns>
	tinyxml2::XMLElement* subNode(string& object,tinyxml2::XMLDocument& doc);
	template<class T>tinyxml2::XMLElement* subNode(T& object, tinyxml2::XMLDocument& doc);
	template<class T>tinyxml2::XMLElement* subNode(vector<T>& object, tinyxml2::XMLDocument& doc);
	template<class T>tinyxml2::XMLElement* subNode(list<T>& object, tinyxml2::XMLDocument& doc);
	template<class T>tinyxml2::XMLElement* subNode(set<T>& object, tinyxml2::XMLDocument& doc);
	template<class T1,class T2>tinyxml2::XMLElement* subNode(pair<T1, T2>& object, tinyxml2::XMLDocument& doc);
	template<class T1, class T2>tinyxml2::XMLElement* subNode(map<T1, T2>& object, tinyxml2::XMLDocument& doc);
	/// <summary>
	/// Serialize object into xml file with the object name and file name
	/// </summary>
	/// <param name="object"> Object to be serialized</param>
	/// <param name="name"> object name in the xml file</param>
	/// <param name="filename"> file address </param>
	void serialize(std::string& object, const char* name, const char* filename);
	template<class T> void serialize(T& object, const char* name, const char* filename);
	template<class T1> void serialize(std::set<T1>& object, const char* name, const char* filename);
	template<class T1> void serialize(std::vector<T1>& object, const char* name, const char* filename);
	template <class T1> void serialize(std::list<T1>& object, const char* name, const char* filename);
	template <class T1, class T2> void serialize(std::pair<T1, T2>& object, const char* name, const char* filename);
	template <class T1, class T2> void serialize(std::map<T1, T2>& object, const char* name, const char* filename);
	
	/// <summary>
	//	A sub-objct be constructed from the node
	/// </summary>
	/// <param name="object"> object to be recovered</param>
	/// <param name="Node"> Node read from xml file</param>
	void _reconstruct(std::string& object, tinyxml2::XMLElement* Node);
	template<class T> void _reconstruct(T& object,tinyxml2::XMLElement* Node);
	template<class T1> void _reconstruct(std::set<T1>& object,tinyxml2::XMLElement* Node);
	template<class T1> void _reconstruct(std::vector<T1>& object,tinyxml2::XMLElement* Node);
	template <class T1> void _reconstruct(std::list<T1>& object,tinyxml2::XMLElement* Node);
	template <class T1, class T2> void _reconstruct(std::pair<T1, T2>& object, tinyxml2::XMLElement* Node);
	template <class T1, class T2> void _reconstruct(std::map<T1, T2>& object, tinyxml2::XMLElement* Node);

	/// <summary>
	/// deserialize with name and file name
	/// </summary>
	/// <param name="object">to be reconstructed</param>
	/// <param name="name"> name when serialized </param>
	/// <param name="filename"> file address </param>
	void deserialize(std::string& object, const char* name, const char* filename);
	template<class T> void deserialize(T& object, const char* name, const char* filename);
	template<class T1> void deserialize(std::set<T1>& object, const char* name, const char* filename);
	template<class T1> void deserialize(std::vector<T1>& object, const char* name, const char* filename);
	template <class T1> void deserialize(std::list<T1>& object, const char* name, const char* filename);
	template <class T1, class T2> void deserialize(std::pair<T1, T2>& object, const char* name, const char* filename);
	template <class T1, class T2> void deserialize(std::map<T1, T2>& object, const char* name, const char* filename);


	tinyxml2::XMLElement* subNode(string& object, tinyxml2::XMLDocument& doc) {
		// Create a new Element in xml document
		tinyxml2::XMLElement* thisElement = doc.NewElement("");
		// Set the attribute value
		if (!is_bin)	thisElement->SetAttribute("val", object.c_str());
		else thisElement->SetAttribute("val", StrToBase64(object.c_str()));
		return thisElement;
	}
	
	template<class T> tinyxml2::XMLElement* subNode(T& object, tinyxml2::XMLDocument& doc) {
		// Create a new Element in xml document
		tinyxml2::XMLElement* thisElement = doc.NewElement("");
		// Set the attribute value
		thisElement->SetAttribute("val", object);
		return thisElement;
	}

	template<class T> tinyxml2::XMLElement* subNode(vector<T>& object, tinyxml2::XMLDocument& doc) {
		tinyxml2::XMLElement* thisElement = doc.NewElement("");
		int j = 0;
		// recursively create new node for every elements in container
		for (typename vector<T>::iterator i = object.begin(); i != object.end(); i++) {
			// index for readability
			string index = "vector" + to_string(j++);
			tinyxml2::XMLElement* tempElement = subNode(*i, doc);
			tempElement->SetName(index.c_str());
			thisElement->InsertEndChild(tempElement);
		}
		return thisElement;
	}
	
	template<class T> tinyxml2::XMLElement* subNode(list<T>& object, tinyxml2::XMLDocument& doc) {
		tinyxml2::XMLElement* thisElement = doc.NewElement("");
		int j = 0;
		for (typename list<T>::iterator i = object.begin(); i != object.end(); i++) {
			string index = "list" + to_string(j++);
			tinyxml2::XMLElement* tempElement = subNode(*i, doc);
			tempElement->SetName(index.c_str());
			thisElement->InsertEndChild(tempElement);
		}
		return thisElement;
	}
	
	template<class T> tinyxml2::XMLElement* subNode(set<T>& object, tinyxml2::XMLDocument& doc) {
		tinyxml2::XMLElement* thisElement = doc.NewElement("");
		int j = 0;
		for (typename set<T>::iterator i = object.begin(); i != object.end(); i++) {
			string index = "set" + to_string(j++);
			tinyxml2::XMLElement* tempElement = subNode(*i, doc);
			tempElement->SetName(index.c_str());
			thisElement->InsertEndChild(tempElement);
		}
		return thisElement;
	}
	
	template<class T1, class T2> tinyxml2::XMLElement* subNode(pair<T1, T2>& object, tinyxml2::XMLDocument& doc) {
		tinyxml2::XMLElement* thisElement = doc.NewElement("");
		tinyxml2::XMLElement* firstElement = subNode(object.first);
		tinyxml2::XMLElement* secondElement = subNode(object.second);
		firstElement->SetName("first");
		secondElement->SetName("second");
		thisElement->InsertEndChild(firstElement);
		thisElement->InsertEndChild(secondElement);
		return thisElement;
	}
	
	template<class T1, class T2> tinyxml2::XMLElement* subNode(map<T1, T2>& object, tinyxml2::XMLDocument& doc) {
		tinyxml2::XMLElement* thisElement = doc.NewElement("");
		int j = 0;
		for (typename map<T1, T2>::iterator i = object.begin(); i != object.end(); i++) {
			string index = "map" + to_string(j++);

			tinyxml2::XMLElement* firstElement = subNode((*i).first, doc);
			tinyxml2::XMLElement* secondElement = subNode((*i).second, doc);
			firstElement->SetName("first");
			secondElement->SetName("second");
			tinyxml2::XMLElement* tempElement = doc.NewElement(index.c_str());
			tempElement->InsertEndChild(firstElement);
			tempElement->InsertEndChild(secondElement);
			thisElement->InsertEndChild(tempElement);
		}
		return thisElement;
	}




	template<class T> void serialize(T& object, const char* name, const char* filename) {
		if (!is_trivially_copyable<T>::value) {
			//Whether T is copyable
			throw no_template_out_data;
		}
		else {
			// serialization in xml, basic without sub node
			tinyxml2::XMLDocument doc;
			tinyxml2::XMLElement* rootElement = doc.NewElement("serialization");
			tinyxml2::XMLElement* arithmeticElement = doc.NewElement(name);
			arithmeticElement->SetAttribute("val", object);
			rootElement->InsertEndChild(arithmeticElement);
			doc.InsertEndChild(rootElement);
			doc.SaveFile(filename);
		}

	}

	void serialize(std::string& object, const char* name, const char* filename) {
		// serialization in xml, basic without sub node
		// create a document
		tinyxml2::XMLDocument doc;
		// root element = "serialization"
		tinyxml2::XMLElement* rootElement = doc.NewElement("serialization");
		tinyxml2::XMLElement* stringElement = doc.NewElement(name);
		// For binary mode and normal mode
		if (!is_bin)	stringElement->SetAttribute("val", object.c_str());
		else stringElement->SetAttribute("val", StrToBase64(object.c_str()));
		rootElement->InsertEndChild(stringElement);
		doc.InsertEndChild(rootElement);
		doc.SaveFile(filename);
		return;
	}

	template<class T1> void serialize(std::set<T1>& object, const char* name, const char* filename) {
		tinyxml2::XMLDocument doc;
		tinyxml2::XMLElement* rootElement = doc.NewElement("serialization");
		tinyxml2::XMLElement* setElement = doc.NewElement(name);
		int j = 0;
		// For every element in container, create subnode
		for (typename set<T1>::iterator i = object.begin(); i != object.end(); i++) {
			string index = "set";
			index += to_string(j++);
			tinyxml2::XMLElement* tempElement = subNode(*i, doc);
			tempElement->SetName(index.c_str());
			setElement->InsertEndChild(tempElement);
		}
		// insert by order
		rootElement->InsertEndChild(setElement);
		doc.InsertEndChild(rootElement);
		// cover the exsisting file
		doc.SaveFile(filename);
	}

	template<class T1> void serialize(std::vector<T1>& object, const char* name, const char* filename) {
		tinyxml2::XMLDocument doc;
		tinyxml2::XMLElement* rootElement = doc.NewElement("serialization");
		tinyxml2::XMLElement* vectorElement = doc.NewElement(name);
		int j = 0;
		for (typename vector<T1>::iterator i = object.begin(); i != object.end(); i++) {
			string index = "vector";
			index += to_string(j++);
			tinyxml2::XMLElement* tempElement = subNode(*i, doc);
			tempElement->SetName(index.c_str());
			vectorElement->InsertEndChild(tempElement);
		}
		rootElement->InsertEndChild(vectorElement);
		doc.InsertEndChild(rootElement);
		doc.SaveFile(filename);
	}

	template <class T1> void serialize(std::list<T1>& object, const char* name, const char* filename) {
		tinyxml2::XMLDocument doc;
		tinyxml2::XMLElement* rootElement = doc.NewElement("serialization");
		tinyxml2::XMLElement* listElement = doc.NewElement(name);
		int j = 0;
		for (typename list<T1>::iterator i = object.begin(); i != object.end(); i++) {
			string index = "list";
			index += to_string(j++);
			tinyxml2::XMLElement* tempElement = subNode(*i, doc);
			tempElement->SetName(index.c_str());
			listElement->InsertEndChild(tempElement);
		}
		rootElement->InsertEndChild(listElement);
		doc.InsertEndChild(rootElement);
		doc.SaveFile(filename);
	}

	template <class T1, class T2> void serialize(std::pair<T1, T2>& object, const char* name, const char* filename) {
		// Base Document
		tinyxml2::XMLDocument doc;
		//const char* declaration = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>";
		//doc.Parse(declaration);
		//root
		tinyxml2::XMLElement* rootElement = doc.NewElement("serialization");
		//pair name
		tinyxml2::XMLElement* pairElement = doc.NewElement(name);
		//first, second 

		tinyxml2::XMLElement* firstElement = subNode(object.first, doc);
		tinyxml2::XMLElement* secondElement = subNode(object.second, doc);
		firstElement->SetName("first");
		secondElement->SetName("second");

		//construct xml doc
		pairElement->InsertEndChild(firstElement);
		pairElement->InsertEndChild(secondElement);
		rootElement->InsertEndChild(pairElement);
		doc.InsertEndChild(rootElement);
		doc.SaveFile(filename);
		return;
	}
	
	template <class T1, class T2> void serialize(std::map<T1, T2>& object, const char* name, const char* filename) {
		tinyxml2::XMLDocument doc;
		tinyxml2::XMLElement* rootElement = doc.NewElement("serialization");
		tinyxml2::XMLElement* mapElement = doc.NewElement(name);
		int j = 0;
		for (typename map<T1, T2>::iterator i = object.begin(); i != object.end(); i++) {
			string index = "map";
			index += to_string(j++);
			tinyxml2::XMLElement* pairElement = doc.NewElement(index.c_str());
			tinyxml2::XMLElement* keyElement = subNode((*i).first, doc);
			tinyxml2::XMLElement* valueElement= subNode((*i).second, doc);
			keyElement->SetName("first");
			valueElement->SetName("second");
			pairElement->InsertEndChild(keyElement);
			pairElement->InsertEndChild(valueElement);
			mapElement->InsertEndChild(pairElement);
		}
		rootElement->InsertEndChild(mapElement);
		doc.InsertEndChild(rootElement);
		doc.SaveFile(filename);
	}




	
	void _reconstruct(std::string& object, tinyxml2::XMLElement* Node) {
		// rebuild from the Node in xml document
		const char* _value = Node->FindAttribute("val")->Value();
		// for binary mode
		if (is_bin)_value = Base64ToStr(_value);
		object = _value;
	}
	
	template<class T> void _reconstruct(T& object, tinyxml2::XMLElement* Node) {
		const char* _value  = Node->FindAttribute("val")->Value();
		object = atof(_value);
	}

	template<class T1> void _reconstruct(std::vector<T1>& object, tinyxml2::XMLElement* Node) {
		// use loop to rebuild a container
		object.clear();
		for(tinyxml2::XMLElement* now = Node->FirstChildElement();now!=0;now=now->NextSiblingElement()){
			T1 temp;
			_reconstruct(temp, now);
			object.push_back(temp);
		}
	}
	
	template<class T1> void _reconstruct(std::set<T1>& object, tinyxml2::XMLElement* Node) {
		object.clear();
		for (tinyxml2::XMLElement* now = Node->FirstChildElement(); now != 0; now = now->NextSiblingElement()) {
			T1 temp;
			_reconstruct(temp, now);
			object.push_back(temp);
		}
	}
	
	template<class T1> void _reconstruct(std::list<T1>& object, tinyxml2::XMLElement* Node) {
		object.clear();
		for (tinyxml2::XMLElement* now = Node->FirstChildElement(); now != 0; now = now->NextSiblingElement()) {
			T1 temp;
			_reconstruct(temp, now);
			object.push_back(temp);
		}
	}
	
	template<class T1, class T2> void _reconstruct(std::map<T1, T2>& object, tinyxml2::XMLElement* Node) {
		map<T1, T2> result;
		for (tinyxml2::XMLElement* now = Node->FirstChildElement(); now != 0; now = now->NextSiblingElement()) {
			T1 first;
			T2 second;
			tinyxml2::XMLElement* firstElement = now->FirstChildElement("first");
			_reconstruct(first, firstElement);
			tinyxml2::XMLElement* secondElement = now->FirstChildElement("second");
			_reconstruct(second, secondElement);
			pair<T1, T2> temp = { first,second };
			result.insert(temp);
		}
		object = result;
	}
	
	template<class T1, class T2> void _reconstruct(std::pair<T1, T2>& object, tinyxml2::XMLElement* Node) {
		T1 first;
		T2 second;
		tinyxml2::XMLElement* firstElement = Node->FirstChildElement("first");
		_reconstruct(first, firstElement);
		tinyxml2::XMLElement* secondElement = Node->FirstChildElement("second");
		_reconstruct(second, secondElement);
		pair<T1, T2> result = { first,second };
		object = result;
	}



	
	void deserialize(std::string& object, const char* name, const char* filename) {
		tinyxml2::XMLDocument doc;
		// file open failure
		if (doc.LoadFile(filename) != 0)
		{
			cout << "load xml file failed" << endl;
			return;
		}
		// for string, no need to recursively rebuild
		tinyxml2::XMLElement* root = doc.RootElement();
		tinyxml2::XMLElement* strNode = root->FirstChildElement(name);
		const char* _value = strNode->FindAttribute("val")->Value();
		if (is_bin)_value = Base64ToStr(_value);
		object = _value;
		return;
	}
	
	template<class T> void deserialize(T& object, const char* name, const char* filename) {
		tinyxml2::XMLDocument doc;
		if (doc.LoadFile(filename) != 0)
		{
			cout << "load xml file failed" << endl;
			return;
		}
		tinyxml2::XMLElement* root = doc.RootElement();
		tinyxml2::XMLElement* strNode = root->FirstChildElement(name);
		object = atof(strNode->FindAttribute("val")->Value());
		return;
	}
	
	template<class T1>	void deserialize(std::vector<T1>& object, const char* name, const char* filename) {
		tinyxml2::XMLDocument doc;
		if (doc.LoadFile(filename) != 0)
		{
			cout << "load xml file failed" << endl;
			return;
		}
		tinyxml2::XMLElement* root = doc.RootElement();
		vector<T1> result;
		tinyxml2::XMLElement* vecNode = root->FirstChildElement(name);
		//rebuild
		_reconstruct(result, vecNode);
		object = result;
	}
	
	template<class T1>	void deserialize(std::set<T1>& object, const char* name, const char* filename) {
		tinyxml2::XMLDocument doc;
		if (doc.LoadFile(filename) != 0)
		{
			cout << "load xml file failed" << endl;
			return;
		}
		tinyxml2::XMLElement* root = doc.RootElement();
		vector<T1> result;
		tinyxml2::XMLElement* setNode = root->FirstChildElement(name);
		_reconstruct(result, setNode);
		object = result;
	}
	
	template <class T1>	void deserialize(std::list<T1>& object, const char* name, const char* filename) {
		tinyxml2::XMLDocument doc;
		if (doc.LoadFile(filename) != 0)
		{
			cout << "load xml file failed" << endl;
			return;
		}
		tinyxml2::XMLElement* root = doc.RootElement();
		list<T1> result;
		tinyxml2::XMLElement* listNode = root->FirstChildElement(name);
		_reconstruct(result, listNode);
		object = result;
	}
	
	template <class T1, class T2>	void deserialize(std::pair<T1, T2>& object, const char* name, const char* filename) {
		tinyxml2::XMLDocument doc;
		if (doc.LoadFile(filename) != 0)
		{
			cout << "load xml file failed" << endl;
			return;
		}
		tinyxml2::XMLElement* root = doc.RootElement();
		pair<T1,T2> result;
		tinyxml2::XMLElement* pairNode = root->FirstChildElement(name);
		_reconstruct(result, pairNode);
		object = result;
	}
	
	template <class T1, class T2>	void deserialize(std::map<T1, T2>& object, const char* name, const char* filename) {
		tinyxml2::XMLDocument doc;
		if (doc.LoadFile(filename) != 0)
		{
			cout << "load xml file failed" << endl;
			return;
		}
		tinyxml2::XMLElement* root = doc.RootElement();
		map<T1, T2> result;
		tinyxml2::XMLElement* mapNode = root->FirstChildElement(name);
		_reconstruct(result, mapNode);
		object = result;
	}
	


};

