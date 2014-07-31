//!
/// brief: strutil.h extern std::string function.
///

#if !defined(cclib_str_util_h_)
#define cclib_str_util_h_

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <typeinfo>  /// -> for bad_cast exception.

namespace cclib {

//!
/// brief: string_cast<T>(str) template function
///          cast str to special type.
/// usage: int x = string_cast<int>("123456");
/// WARNING: if cast failure, then throw bad_cast exception.
template<typename T>
inline T string_cast(std::string const& str)
{
    std::stringstream ss(str);
    T ret;
    if (ss >> ret) return ret;

    // if cast failure then throw bad_cast exception.
    std::string err = "string: \"";
    err += str;
    err += "\" cast to type: ";
    err += typeid(T).name();
    err += " failure!";
    
    throw std::bad_cast(err.c_str());
}

//!
/// brief: to_string(T) template function
///          change t to string format.
/// usage: int x = string_cast<int>("123456");
/// WARNING: if cast failure, then throw bad_cast exception.
template<typename T>
inline std::string to_string(T value)
{
    std::stringstream ss;
    std::string ret;

    ss << value;
    if (ss >> ret) return ret;

    // if cast failure then throw bad_cast exception.
    std::string err = "change type: ";
    err += typeid(T).name();
    err += " to string failure!";
    
    throw std::bad_cast(err.c_str());
}

//!
/// brief: one string change to lower
///        only change A~Z to a~z, digit 0~9 and other charactor NEVER change.
/// usage: string lower = to_lower("AbcdefGH0123!#@");
///        lower is: "abcdefgh0123!#@";
inline std::string to_lower(std::string const& str)
{
    std::string tmp = str;
    std::string::size_type i;
    for (i = 0; i < tmp.length(); ++i) {
        if (tmp[i] >= 'A' && tmp[i] <= 'Z')
            tmp[i] = tmp[i] - 'A' + 'a';
    }

    return tmp;
}

//!
/// brief: one string change to higher
///        only change a~z to A~Z, digit 0~9 and other charactor NEVER change.
/// usage: string higher = to_lower("AbcdefGH0123!#@");
///        higher is: "ABCDEFGH0123!#@";
inline std::string to_higher(std::string const& str)
{
    std::string tmp = str;
    std::string::size_type i;
    for (i = 0; i < tmp.length(); ++i) {
        if (tmp[i] >= 'a' && tmp[i] <= 'z')
            tmp[i] = tmp[i] - 'a' + 'A';
    }

    return tmp;
}

//!
/// brief: split string by separetor.
///        
/// usage: 
///      1. std::vector<std::string> out;
///         size_t sz = split("c:\\xxx\\yyy\\\\zzz.txt", "\\", &out);
///         result: sz = out.size(); sz = 5.
///                 out:[0] = "c:", [1] = "xxx", [2] = "yyy", 
///                     [3] = "",   [4] = "zzz.txt";
///
///      2. std::vector<std::string> out;
///         size_t sz = split("c:\\xxx\\yyy\\\\zzz.txt", "\\", &out, true);
///         result: sz = out.size(); sz = 4.
///                 out:[0] = "c:", [1] = "xxx", [2] = "yyy", 
///                     [3] = "zzz.txt";
/// notice: trim_empty will trim empty string in output vector.
inline size_t split(
    std::string const& str, 
    std::string const& sp, 
    std::vector<std::string> *out, 
    bool trim_empty = false)
{
    std::string::size_type pos1, pos2;
    // for cache split strings, and append to out:<std::vector<std::string>>.
    std::vector<std::string> tmpout;
    std::string tmp;
 
    pos1 = 0;
    while ((pos2 = str.find(sp, pos1)) != std::string::npos) {
        tmp = str.substr(pos1, pos2 - pos1);
        if (tmp.empty() && trim_empty)
            continue;
        tmpout.push_back(tmp);
        pos1 = pos2 + sp.length();
    }
    tmpout.push_back(str.substr(pos1));

    std::vector<std::string>::iterator it;
    for (it = tmpout.begin(); it != tmpout.end(); ++it)
        out->push_back(*it);

    return tmpout.size();
}

//!
/// brief: split string likely "key1=value1&key2=value2..." to
///        map["key1"] = "value1", map["key2"] = "value2".
/// usage: 
///      1. std::map<std::string, std::string> out;
///         size_t sz = split("-A=127.0.0.1 -p=6666 -N=test", " ", "=", &out);
///         result: sz = out.size(); sz = 3;
///                 out:[0] = <"-A", "127.0.0.1">, [1] = <"-p", "6666">
///                     [2] = <"-N", "test">;
///      2. std::map<std::string, std::string> out;
///         size_t sz = split("-A=127.0.0.1 -p=6666 -N=test", " ", "=", &out, true);
///         result: sz = out.size(); sz = 3;
///                 out:[0] = <"-a", "127.0.0.1">, [1] = <"-p", "6666">
///                     [2] = <"-n", "test">;
///      3. std::map<std::string, std::string> out;
///         size_t sz = split("-A=127.0.0.1 -p=6666 -N=test NoSatisfiedRule", " ", "=", &out, true);
///         result: sz = out.size(); sz ISN'T 4 BUT 0, sz = 0.
///                 ??? out:[0] = <"-a", "127.0.0.1">, [1] = <"-p", "6666">
///                         [2] = <"-n", "test">;      [3] = ???HOW TO RESOLVE.
///         NOTICE: every section must statisfaied rule.
inline size_t split(
    std::string const& str, 
    std::string const& sp1, std::string const& sp2, 
    std::map<std::string, std::string> *out,
    bool key_lower = false)
{
    std::vector<std::string> strvec;
    // first split by sp1.
    split(str, sp1, &strvec);

    std::map<std::string, std::string> kv_map;
    std::string::size_type pos;
    for (size_t i = 0; i < strvec.size(); ++i) {
        pos = strvec[i].find(sp2);
        if (std::string::npos == pos) {
            //HACK:: if string not statisfied the rule, 
            //       then return null map.
            //IMPROVE:: throw exception? bad_cast???
            kv_map.clear();
            break;
        }
        if (key_lower)
            kv_map[to_lower(strvec[i].substr(0, pos))] = strvec[i].substr(pos + sp2.length());
        else
            kv_map[strvec[i].substr(0, pos)] = strvec[i].substr(pos + sp2.length());
    }
    // append to out map.
    out->insert(kv_map.begin(), kv_map.end());

    return kv_map.size();
}

//!
/// brief: split string likely "{section1} {section2}..." to
///        "section1", "section2";
/// usage: 
///      1. std::vector<std::string> out;
///         size_t sz = split("{first args} {second args} {} {", "{", "}", &out);
///         result: sz = out.size(); sz = 3;
///                 out:[0] = "first args", [1] = "second args",
///                     [2] = "";
///      2. std::vector<std::string> out;
///         size_t sz = split("{first args} {second args} {} {", "{", "}", &out, true);
///         result: sz = out.size(); sz = 2;
///                 out:[0] = "first args", [1] = "second args",
inline size_t split(
    std::string const& str, 
    std::string const& sp1, std::string const& sp2, 
    std::vector<std::string> *out,
    bool trim_empty = false)
{
    std::string::size_type begin, end;
    // for cache split strings, and append to out:<std::vector<std::string>>.
    std::vector<std::string> tmpout;
    std::string tmp;
 
    begin = 0;
    while ((begin = str.find(sp1, begin)) != std::string::npos) {
        if ((end = str.find(sp2, begin + 1)) == std::string::npos) 
            break;
        // find end str
        tmp = str.substr(begin + 1, end - begin - 1);
        if (tmp.empty() && trim_empty)
            continue;
        tmpout.push_back(tmp);
        begin = end + 1;
    }

    std::vector<std::string>::iterator it;
    for (it = tmpout.begin(); it != tmpout.end(); ++it)
        out->push_back(*it);

    return tmpout.size();
}

const std::string kSpace     = " ";
const std::string kTab       = "\t";
const std::string kEnter     = "\r";
const std::string kNewLine   = "\n";
const std::string kAllTrimed = "\r\n\t ";

//!
/// brief: trim string with "\r\n\t " string.
///        only trim start and end, if this string in the middle.
///        then it is reserved.
/// usage: 
///      1. std::string trimstr = trim("\r\t\nthis is a test string, \nhello, world!\r\t\n");
///         result: trimstr is: "this is a test string, \nhello, world!";
///      2. std::string trimstr = trim(" \r\t\nthis is a test string, \rhello, world!\r\t\n ", kSpace);
///         result: trimstr is: "\r\t\nthis is a test string, \rhello, world!\r\t\n";
inline std::string trim(
    std::string const& str, std::string const& trim_str = kAllTrimed)
{
    if (str.length() == 0)
        return "";

    size_t start = str.length(), end = str.length();
    bool started = false;
    bool is_trimed_str;
    for (size_t i = 0; i < str.length(); ++i) {
        is_trimed_str = false;
        for (size_t j = 0; j < trim_str.length(); ++j) {
            if (str[i] == trim_str[j]) {
                is_trimed_str = true;
                continue;
            }
        }

        if (is_trimed_str)
            continue;

        // get the start trim pos;
        if (!started) {
            start = i;
            started = true;
        }

        // get the end trim pos.
        end = i;
    }

    if (start == str.length())
        return "";

    return str.substr(start, end - start + 1);
}

/// TODO:: no test.
inline bool start_with(
    std::string const& str, std::string const& with, 
    bool case_sensitive = true)
{
    /// if case sensitive.
    if (case_sensitive) 
        return str.find(with) == 0;

    /// if case insensitive, 
    ///   first change 'str' and 'with' to lower or higher, 
    ///   then find.
    std::string tmpstr = to_lower(str);
    std::string tmpwith = to_lower(with);

    return tmpstr.find(tmpwith) == 0;
}

//!
/// TODO:: no test.
inline bool end_with(
    std::string const& str, std::string const& with,
    bool case_sensitive = true)
{
    /// if case sensitive.
    if (case_sensitive) 
        return str.rfind(with) == (str.length() - with.length());

    /// if case insensitive, 
    ///   first change 'str' and 'with' to lower or higher, 
    ///   then find.
    std::string tmpstr  = to_lower(str);
    std::string tmpwith = to_lower(with);

    return tmpstr.rfind(tmpwith) == (tmpstr.length() - tmpwith.length());
}

//!
/// brief: replace 'from' to 'to'
inline int replace(
    std::string& str, 
    std::string const& from, std::string const& to)
{
    int times = 0;
    size_t index = 0;
    while (true) {
         /* Locate the substring to replace. */
         index = str.find(from, index);
         if (index == std::string::npos) break;

         /* Make the replacement. */
         str.replace(index, from.length(), to);

         /* record replace times */
         times++;

         /* Advance index forward so the next iteration doesn't pick it up as well. */
         index += from.length();
    }
    return times;
}

} // namespace cclib

#endif // cclib_str_util_h_