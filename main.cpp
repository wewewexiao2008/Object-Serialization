#include <iostream>
#include <ctime>
#include "XMLserial.h"
#include "binary_serialization.h"
using namespace std;

/*
    The User should define the functions below to make the struct serializable
    Make the functions in the namespace,
    we can serialize the Nested of the UserDefinedType and STL containers and arithmatic
    such as Vector<UserDefinedType> and so on
*/

struct UserDefinedType {
    int idx;
    std::string name;
    std::vector<double> data;
};

namespace binary_serialization{
    template <> void in_data(fstream & fin, UserDefinedType & object)
    {
        in_data(fin, object.idx);
        in_data(fin, object.name);
        in_data(fin, object.data);
    }
    template <> void out_data(fstream & fout, UserDefinedType & object)
    {
        out_data(fout, object.idx);
        out_data(fout, object.name);
        out_data(fout, object.data);
    }
};

namespace xml_serialization {
    void _reconstruct(UserDefinedType& object, tinyxml2::XMLElement* Node) {
        // partition into single parts and rebuild
        _reconstruct(object.idx, Node->FirstChildElement("idx"));
        _reconstruct(object.name, Node->FirstChildElement("name"));
        _reconstruct(object.data, Node->FirstChildElement("data"));
    }

    tinyxml2::XMLElement* subNode(UserDefinedType& object, tinyxml2::XMLDocument& doc) {
        // For each sub part of this UDT, we separate them into the existing parts.
        tinyxml2::XMLElement* thisElement = doc.NewElement("");
        tinyxml2::XMLElement* idxElement = doc.NewElement("idx");
        tinyxml2::XMLElement* nameElement = doc.NewElement("name");
        tinyxml2::XMLElement* dataElement = doc.NewElement("data");
        idxElement->SetAttribute("val", object.idx);
        if (!is_bin) nameElement->SetAttribute("val", object.name.c_str());
        else nameElement->SetAttribute("val", StrToBase64(object.name.c_str()));
        int j = 0;
        for (typename vector<double>::iterator i = object.data.begin(); i != object.data.end(); i++) {
            string index = "vector";
            index += to_string(j++);
            tinyxml2::XMLElement* tempElement = subNode(*i, doc);
            tempElement->SetName(index.c_str());
            dataElement->InsertEndChild(tempElement);
        }
        thisElement->InsertEndChild(idxElement);
        thisElement->InsertEndChild(nameElement);
        thisElement->InsertEndChild(dataElement);
        return thisElement;
    }

    void serialize( UserDefinedType & object, const char* name, const char* filename) {
        tinyxml2::XMLDocument doc;
        // create node
        tinyxml2::XMLElement* rootElement = doc.NewElement("serialization");
        tinyxml2::XMLElement* userElement = doc.NewElement(name);
        tinyxml2::XMLElement* idxElement = doc.NewElement("idx");
        tinyxml2::XMLElement* nameElement = doc.NewElement("name");
        tinyxml2::XMLElement* dataElement = doc.NewElement("data");
        // For UDT, partition it into parts
        //idx
        idxElement->SetAttribute("val", object.idx);
        //name
        if (!is_bin)	nameElement->SetAttribute("val", object.name.c_str());
        else nameElement->SetAttribute("val", StrToBase64(object.name.c_str()));
        int j = 0;
        //data vector
        for (typename vector<double>::iterator i = object.data.begin(); i != object.data.end(); i++) {
            string index = "vector";
            index += to_string(j++);
            tinyxml2::XMLElement* tempElement = subNode(*i, doc);
            tempElement->SetName(index.c_str());
            dataElement->InsertEndChild(tempElement);
        }
        userElement->InsertEndChild(idxElement);
        userElement->InsertEndChild(nameElement);
        userElement->InsertEndChild(dataElement);
        rootElement->InsertEndChild(userElement);
        doc.InsertEndChild(rootElement);
        doc.SaveFile(filename);
    }

    void deserialize(UserDefinedType& object, const char* name, const char* filename) {
        tinyxml2::XMLDocument doc;
        if (doc.LoadFile(filename) != 0)
        {
            cout << "load xml file failed" << endl;
            return;
        }
        tinyxml2::XMLElement* root = doc.RootElement();
        tinyxml2::XMLElement* userNode = root->FirstChildElement(name);
        //reverse of serialization
        _reconstruct(object, userNode);
    }
}

/*
    There is no need to add function for serialization.
    Because this class is the collection of arithmatic
    As a result, it's can be serialized by serialize the memory
*/

class arithmatic_collection{
    private:
        int i;
        float f;
        double d;
        long l;
        long long ll;
        bool b;
    public:
        void build(){
            i = 1;
            f = 2.0;
            d = 3.0;
            l = 4;
            ll = 5LL;
            b = true;
        }
        friend ostream& operator << (ostream& output, arithmatic_collection& c);
};

/*
    Overload operator for check the answer.
*/

ostream& operator << (ostream& output, arithmatic_collection& c)
{
    output<<" i = "<<c.i<<" f = "<<c.f<<" d = "<<c.d<<" l = "<<c.l<<" ll = "<<c.ll<<" b = "<<c.b;
    return output;
}

template <class T1,class T2>
ostream& operator << (ostream& output, map <T1, T2>& c)
{
    output<<"Map : "<<c.size()<<" ";
    for (auto i=c.begin(); i!=c.end(); ++i)
        output<< i->first <<"-"<<i->second<<" ";
    return output;
}

template <class T>
ostream& operator << (ostream& output, vector <T>& c)
{
    output<<"Vector : "<<c.size()<<"  ";
    for (int i=0; i<c.size();++i)
        output<<c[i]<<" ";
    return output;
}

template <class T>
ostream& operator << (ostream& output, vector <vector <T> >& c)
{
    output<<"Vector : "<<c.size()<<"  "<<endl;
    for (int i=0; i<c.size();++i)
        output<<" "<<c[i]<<endl;
    return output;
}

ostream& operator << (ostream& output, UserDefinedType& c)
{
    output<<" idx = "<<c.idx<<" name = "<<c.name;
    cout<<" Data: ";
    for (int i=0; i<c.data.size(); ++i)
        output<<c.data[i]<<" ";
    return output;
}

int main()
{
    try{
        /*
            Case $1 Binary Serialization
        */

        /*
            Test #1 Integer
            Result: Accepted
        */
        cout<<"===============Case $1 Binary Serialization==============="<<endl;
        cout<<"Test #1 Integer:"<<endl;
        int ia = 1, ib = 0, ic = 0;
        binary_serialization::serialize(ia, "test.txt");
        binary_serialization::deserialize(ib, "test.txt");
        binary_serialization::deserialize(ic, "test.txt");
        cout<<" ia = "<<ia<<endl;
        cout<<" ib = "<<ib<<endl;
        cout<<" ic = "<<ic<<endl;
        cout<<"---------------------"<<endl;

        /*
            Test #2 Float
            Result: Accepted
        */

        cout<<"Test #2 Float:"<<endl;
        float fa = 2.0, fb = 0, fc = 0;
        binary_serialization::serialize(fa, "test.txt");
        binary_serialization::deserialize(fb, "test.txt");
        binary_serialization::deserialize(fc, "test.txt");
        cout<<" fa = "<<fa<<endl;
        cout<<" fb = "<<fb<<endl;
        cout<<" fc = "<<fc<<endl;
        cout<<"---------------------"<<endl;

        /*
            Test #3 Arithmatic Collection
            This Serialization is not need to realize by User
            We can use is_trivially_copyable to check it and tricvially copy it
            Result: Accepted
        */
        
        cout<<"Test #3 Arithmatic Collection:"<<endl;
        arithmatic_collection aca, acb, acc;
        aca.build();
        binary_serialization::serialize(aca, "test.txt");
        binary_serialization::deserialize(acb, "test.txt");
        binary_serialization::deserialize(acc, "test.txt");
        cout<<aca<<endl;
        cout<<acb<<endl;
        cout<<acc<<endl;
        cout<<"---------------------"<<endl;

        /*
            Test #4 Vector
            Result: Accepted
        */

        cout<<"Test #4 Vector:"<<endl;
        vector <int> via, vib, vic;
        via.clear();
        via.push_back(1); via.push_back(3);
        via.push_back(7); via.push_back(8);
        binary_serialization::serialize(via, "test.txt");
        binary_serialization::deserialize(vib, "test.txt");
        binary_serialization::deserialize(vic, "test.txt");
        cout<<via<<endl;
        cout<<vib<<endl;
        cout<<vic<<endl;
        cout<<"---------------------"<<endl;

        /*
            Test #5 String
            Result: Accepted
        */

        cout<<"Test #5 String:"<<endl;
        string sa, sb, sc;
        sa = "Hello, World!";
        binary_serialization::serialize(sa, "test.txt");
        binary_serialization::deserialize(sb, "test.txt");
        binary_serialization::deserialize(sc, "test.txt");
        cout<<"sa = "<<sa<<endl;
        cout<<"sb = "<<sb<<endl;
        cout<<"sc = "<<sc<<endl;
        cout<<"---------------------"<<endl;

        /*
            Test #6 Pair
            Result: Accepted
        */

        cout<<"Test #6 Pair:"<<endl;
        pair<int,double> pa, pb, pc;
        pa.first = 5; pa.second = 10.0;
        binary_serialization::serialize(pa, "test.txt");
        binary_serialization::deserialize(pb, "test.txt");
        binary_serialization::deserialize(pc, "test.txt");
        cout<<"pa = "<<pa.first<<" "<<pa.second<<endl;
        cout<<"pb = "<<pb.first<<" "<<pb.second<<endl;
        cout<<"pc = "<<pc.first<<" "<<pc.second<<endl;
        cout<<"---------------------"<<endl;

        /*
            Test #7 List
            Result: Accepted
        */

        cout<<"Test #7 List:"<<endl;
        list<int> la={1,3,5,7,9},lb,lc;
        binary_serialization::serialize(la, "test.txt");
        binary_serialization::deserialize(lb, "test.txt");
        binary_serialization::deserialize(lc, "test.txt");
        cout<<"la = ";
        for (auto i = la.begin(); i!=la.end(); ++i)
            cout<<*i<<" ";
        cout<<endl;
        cout<<"lb = ";
        for (auto i = lb.begin(); i!=lb.end(); ++i)
            cout<<*i<<" ";
        cout<<endl;
        cout<<"lc = ";
        for (auto i = lc.begin(); i!=lc.end(); ++i)
            cout<<*i<<" ";
        cout<<endl;
        cout<<"---------------------"<<endl;

        /*
            Test #8 Map
            Result: Accepted
        */
        
        cout<<"Test #8 Map:"<<endl;
        map<int, double> ma,mb,mc;
        ma.clear(); ma[1] = 2.0; ma[4] = 8.0; ma[10] = 20.0;
        binary_serialization::serialize(ma, "test.txt");
        binary_serialization::deserialize(mb, "test.txt");
        binary_serialization::deserialize(mc, "test.txt");
        cout<<"ma = ";
        for (auto i = ma.begin(); i!=ma.end(); ++i)
            cout<<i->first<<"-"<<i->second<<" ";
        cout<<endl;
        cout<<"mb = ";
        for (auto i = mb.begin(); i!=mb.end(); ++i)
            cout<<i->first<<"-"<<i->second<<" ";
        cout<<endl;
        cout<<"mc = ";
        for (auto i = mc.begin(); i!=mc.end(); ++i)
            cout<<i->first<<"-"<<i->second<<" ";
        cout<<endl;
        cout<<"---------------------"<<endl;

        /*
            Test #9 UserDefinedType
            This Serialize is implemented by User
            Result: Accepted
        */

        cout<<"Test #9 UserDefinedType:"<<endl;
        UserDefinedType ua,ub,uc;
        ua.idx = 5;
        ua.name = "Root";
        ua.data.push_back(5);
        ua.data.push_back(10);
        ua.data.push_back(50);
        binary_serialization::serialize(ua, "test.txt");
        binary_serialization::deserialize(ub, "test.txt");
        binary_serialization::deserialize(uc, "test.txt");
        cout<<" ua = "<<ua<<endl;
        cout<<" ua = "<<ub<<endl;
        cout<<" ua = "<<uc<<endl;
        cout<<"---------------------"<<endl;

        /*
            Test #10 Vector<Vector>
            Result: Accepted
        */

        cout<<"Test #10 Vector<Vector>:"<<endl;
        vector<vector<int>> vvia, vvib, vvic;
        vvia.clear();
        vvia.push_back(via);
        via.push_back(9);
        vvia.push_back(via);
        binary_serialization::serialize(vvia, "test.txt");
        binary_serialization::deserialize(vvib, "test.txt");
        binary_serialization::deserialize(vvic, "test.txt");
        cout<<vvia<<endl;
        cout<<vvib<<endl;
        cout<<vvic<<endl;
        cout<<"---------------------"<<endl;

        /*
            Test #11 Unique_ptr
            Result: Accepted
        */

        cout<<"Test #11 Unique_ptr:"<<endl;
        unique_ptr <string> upa (new string("Hello, the world!")), upb, upc;
        binary_serialization::serialize(upa, "test.txt");
        binary_serialization::deserialize(upb, "test.txt");
        binary_serialization::deserialize(upc, "test.txt");
        cout<<"upa -> "<<*upa<<endl;
        cout<<"upb -> "<<*upb<<endl;
        cout<<"upb -> "<<*upc<<endl;
        cout<<"---------------------"<<endl;

        /*
            Test #12 Weak_ptr
            Result: Accepted
        */

        cout<<"Test #12 Weak_ptr:"<<endl;
        unique_ptr <string> wpa (new string("Hello, the world!")), wpb, wpc;
        binary_serialization::serialize(wpa, "test.txt");
        binary_serialization::deserialize(wpb, "test.txt");
        binary_serialization::deserialize(wpc, "test.txt");
        cout<<"upa -> "<<*wpa<<endl;
        cout<<"upb -> "<<*wpb<<endl;
        cout<<"upb -> "<<*wpc<<endl;
        cout<<"---------------------"<<endl;

        /*
            Test #13 Shared_ptr
            Result: Accepted
        */

        cout<<"Test #13 Shared_ptr:"<<endl;
        unique_ptr <string> spa (new string("Hello, the world!")), spb, spc;
        binary_serialization::serialize(spa, "test.txt");
        binary_serialization::deserialize(spb, "test.txt");
        binary_serialization::deserialize(spc, "test.txt");
        cout<<"upa -> "<<*spa<<endl;
        cout<<"upb -> "<<*spb<<endl;
        cout<<"upb -> "<<*spc<<endl;
        cout<<"---------------------"<<endl;

        /*
            Test #14: Vector < Map <int,string> >
            Result:Accepted
        */
        
        cout<<"Test #14 Vector< Map<int,string> >:"<<endl;
        vector<map<int, string>> vmisa, vmisb, vmisc;
        map<int, string> mis;
        mis[-10] = "for - 10";
        vmisa.push_back(mis);
        mis[10] = "for 10";
        vmisa.push_back(mis);
        binary_serialization::serialize(vmisa, "test.txt");
        binary_serialization::deserialize(vmisb, "test.txt");
        binary_serialization::deserialize(vmisc, "test.txt");
        cout<<"vmisa -> "<<vmisa<<endl;
        cout<<"vmisa -> "<<vmisb<<endl;
        cout<<"vmisa -> "<<vmisc<<endl;
        

        /*
            Test #15: Vector < UserDefinedType >
            Result:Accepted
        */
        
        cout<<"Test #15 Vector<UserDefinedType>:"<<endl;
        vector<UserDefinedType> vua, vub, vuc;
        vua.push_back(ua);
        vua.push_back(ua);
        binary_serialization::serialize(vua, "test.txt");
        binary_serialization::deserialize(vub, "test.txt");
        binary_serialization::deserialize(vuc, "test.txt");
        cout<<vua<<endl;
        cout<<vub<<endl;
        cout<<vuc<<endl;
        cout<<"---------------------"<<endl;
        

        /*
            Test #16: Performance Test
            Most of the time is spent at file access
            1000 times serialize and deserialize of UserDefinedType is 760ms
        */

        cout<<"Test #16 Performance Test:"<<endl;
        clock_t time_start=clock();
        ua.idx = 5;
        ua.name = "Root";
        ua.data.push_back(5);
        ua.data.push_back(10);
        ua.data.push_back(50);
        for (int i=0; i<1000; ++i){
            binary_serialization::serialize(ua, "PerformaceTest.txt");
        }
        for (int i=0; i<1000; ++i){
            binary_serialization::deserialize(ub, "PerformaceTest.txt");
        }
        clock_t time_end=clock();
        cout<<" ua = "<<ua<<endl;
        cout<<" ua = "<<ub<<endl;
        cout<<"Time :"<<time_end - time_start<<endl;
        cout<<"---------------------"<<endl;
    }catch (binary_serialization::serial_error Error) {
        cout << "Error : " << Error.what() << endl;
    }
    catch (...) {
        cout << "Unrecognized Error" << endl;
    }

    try{
        /*
            Case $2 XML Serialization
        */

        /*
            Test #1 Integer
            Result: Accepted
        */

        cout << "===============Case $2 XML Serialization===============" << endl;
        cout << "Test #1 Integer:" << endl;
        int ia = 1, ib = 0, ic = 0;
        xml_serialization::serialize(ia, "Integer", "test.xml");
        xml_serialization::deserialize(ib, "Integer", "test.xml");
        xml_serialization::deserialize(ic, "Integer", "test.xml");
        cout << " ia = " << ia << endl;
        cout << " ib = " << ib << endl;
        cout << " ic = " << ic << endl;
        cout << "---------------------" << endl;

        /*
            Test #2 Float
            Result: Accepted
        */

        
        cout << "Test #2 Float:" << endl;
        float fa = 2.0, fb = 0, fc = 0;
        xml_serialization::serialize(fa, "Float", "test.xml");
        xml_serialization::deserialize(fb, "Float", "test.xml");
        xml_serialization::deserialize(fc, "Float", "test.xml");
        cout << " fa = " << fa << endl;
        cout << " fb = " << fb << endl;
        cout << " fc = " << fc << endl;
        cout << "---------------------" << endl;
        
        /*
            Test #3 Vector
            Result: Accepted
        */

        cout << "Test #3 Vector:" << endl;
        vector <int> via, vib, vic;
        via.clear();
        via.push_back(1); via.push_back(3);
        via.push_back(7); via.push_back(8);
        xml_serialization::serialize(via, "Vector", "test.xml");
        xml_serialization::deserialize(vib, "Vector", "test.xml");
        xml_serialization::deserialize(vic, "Vector", "test.xml");
        cout << via << endl;
        cout << vib << endl;
        cout << vic << endl;
        cout << "---------------------" << endl;
        

        /*
            Test #4 String
            Result: Accepted
        */

        
        cout << "Test #4 String:" << endl;
        string sa, sb, sc;
        sa = "Hello, World!";
        xml_serialization::serialize(sa, "String", "test.xml");
        xml_serialization::deserialize(sb, "String", "test.xml");
        xml_serialization::deserialize(sc, "String", "test.xml");
        cout << "sa = " << sa << endl;
        cout << "sb = " << sb << endl;
        cout << "sc = " << sc << endl;
        cout << "---------------------" << endl;
        

        /*
            Test #5 Pair
            Result: Accepted
        */

        
        cout << "Test #5 Pair:" << endl;
        pair<int, double> pa, pb, pc;
        pa.first = 5; pa.second = 10.0;
        xml_serialization::serialize(pa, "Pair", "test.xml");
        xml_serialization::deserialize(pb, "Pair", "test.xml");
        xml_serialization::deserialize(pc, "Pair", "test.xml");
        cout << "pa = " << pa.first << " " << pa.second << endl;
        cout << "pb = " << pb.first << " " << pb.second << endl;
        cout << "pc = " << pc.first << " " << pc.second << endl;
        cout << "---------------------" << endl;
        

        /*
            Test #6 List
            Result: Accepted
        */

        
        cout << "Test #6 List:" << endl;
        list<int> la = { 1,3,5,7,9 }, lb, lc;
        xml_serialization::serialize(la, "List", "test.xml");
        xml_serialization::deserialize(lb, "List", "test.xml");
        xml_serialization::deserialize(lc, "List", "test.xml");
        cout << "la = ";
        for (auto i = la.begin(); i != la.end(); ++i)
            cout << *i << " ";
        cout << endl;
        cout << "lb = ";
        for (auto i = lb.begin(); i != lb.end(); ++i)
            cout << *i << " ";
        cout << endl;
        cout << "lc = ";
        for (auto i = lc.begin(); i != lc.end(); ++i)
            cout << *i << " ";
        cout << endl;
        cout << "---------------------" << endl;
        

        /*
            Test #7 Map
            Result: Accepted
        */

        
        cout << "Test #7 Map:" << endl;
        map<int, double> ma, mb, mc;
        ma.clear(); ma[1] = 2.0; ma[4] = 8.0; ma[10] = 20.0;
        xml_serialization::serialize(ma, "Map", "test.xml");
        xml_serialization::deserialize(mb, "Map", "test.xml");
        xml_serialization::deserialize(mc, "Map", "test.xml");
        cout << "ma = ";
        for (auto i = ma.begin(); i != ma.end(); ++i)
            cout << i->first << "-" << i->second << " ";
        cout << endl;
        cout << "mb = ";
        for (auto i = mb.begin(); i != mb.end(); ++i)
            cout << i->first << "-" << i->second << " ";
        cout << endl;
        cout << "mc = ";
        for (auto i = mc.begin(); i != mc.end(); ++i)
            cout << i->first << "-" << i->second << " ";
        cout << endl;
        cout << "---------------------" << endl;
        

        /*
            Test #8 UserDefinedType
            This Serialize is implemented by User
            Result: Accepted
        */

        
        cout << "Test #8 UserDefinedType:" << endl;
        UserDefinedType ua, ub, uc;
        ua.idx = 5;
        ua.name = "Root";
        ua.data.push_back(5);
        ua.data.push_back(10);
        ua.data.push_back(50);
        xml_serialization::serialize(ua, "User_Type", "test.xml");
        xml_serialization::deserialize(ub, "User_Type", "test.xml");
        xml_serialization::deserialize(uc, "User_Type", "test.xml");
        cout << " ua = " << ua << endl;
        cout << " ua = " << ub << endl;
        cout << " ua = " << uc << endl;
        cout << "---------------------" << endl;
        

        /*
            Test #9 Vector<Vector>
            Result: Accepted
        */

        
        cout << "Test #9 Vector<Vector>:" << endl;
        vector<vector<int>> vvia, vvib, vvic;
        vvia.clear();
        vvia.push_back(via);
        via.push_back(9);
        vvia.push_back(via);
        xml_serialization::serialize(vvia, "Vec_Vec", "test.xml");
        xml_serialization::deserialize(vvib, "Vec_Vec", "test.xml");
        xml_serialization::deserialize(vvic, "Vec_Vec", "test.xml");
        cout << vvia << endl;
        cout << vvib << endl;
        cout << vvic << endl;
        cout << "---------------------" << endl;
        
        /*
            Test #10: Vector < Map <int,string> >
            Result:Accepted
            Now activate the binary mode of the xml serialization
        */
        is_bin = true;
        
        cout << "Test #10 Vector< Map<int,string> >:" << endl;
        vector<map<int, string>> vmisa, vmisb, vmisc;
        map<int, string> mis;
        mis[-10] = "for - 10";
        vmisa.push_back(mis);
        mis[10] = "for 10";
        vmisa.push_back(mis);
        xml_serialization::serialize(vmisa, "Vec_Mapintstring", "test.xml");
        xml_serialization::deserialize(vmisb, "Vec_Mapintstring", "test.xml");
        xml_serialization::deserialize(vmisc, "Vec_Mapintstring", "test.xml");
        cout << "vmisa -> " << vmisa << endl;
        cout << "vmisa -> " << vmisb << endl;
        cout << "vmisa -> " << vmisc << endl;
        
        /*
            Test #11: Performance Test
            Most of the time is spent at file access
            1000 times serialize and deserialize of UserDefinedType is 760ms
        */

        cout << "Test #11 Performance Test:" << endl;
        clock_t time_start = clock();
        ua.idx = 5;
        ua.name = "Root";
        ua.data.push_back(5);
        ua.data.push_back(10);
        ua.data.push_back(50);
        for (int i = 0; i < 1000; ++i) {
            xml_serialization::serialize(ua, "User_Type", "PerformaceTest.xml");
        }
        for (int i = 0; i < 1000; ++i) {
            xml_serialization::deserialize(ub, "User_Type", "PerformaceTest.xml");
        }
        clock_t time_end = clock();

        cout << " ua = " << ua << endl;
        cout << " ub = " << ub << endl;
        cout << "Time :" << time_end - time_start << endl;
        cout << "---------------------" << endl;
        
    } catch (binary_serialization::serial_error Error){
        cout<<"Error : "<<Error.what()<<endl;
    } catch (...){
        cout<<"Unrecognized Error"<<endl;
    }


}