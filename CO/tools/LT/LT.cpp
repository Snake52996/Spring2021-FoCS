#include "tinyxml2.h"
#include<getopt.h>
#include<cstdlib>
#include<cstring>
#include<thread>
#include<cstdio>
#include<filesystem>
#include<mutex>
#include<vector>
#include<cstdio>
#include<fstream>
#include<string>
#include<regex>
#ifdef __linux__
#define REDCOLOR "\e[31;1m"
#define GREENCOLOR "\e[32;1m"
#define YELLOWCOLOR "\e[33;1m"
#define BLUECOLOR "\e[34;1m"
#define PURPLECOLOR "\e[35;1m"
#define AOCOLOR "\e[36;1m"
#define ENDCOLOR "\e[0m"
#define LOGISIM_COMMAND "timeout %d %s -tty table %s > %s 2>/dev/null",config.timeout_,config.logisim_.c_str(),circ_path.c_str(),temp_out_path.c_str()
#else
#define REDCOLOR ""
#define GREENCOLOR ""
#define YELLOWCOLOR ""
#define BLUECOLOR ""
#define PURPLECOLOR ""
#define AOCOLOR ""
#define ENDCOLOR ""
#define LOGISIM_COMMAND "Start-Job { %s -tty table %s > %s 2>/dev/null } | Wait-Job -Timeout %d ",config.logisim_.c_str(),circ_path.c_str(),temp_out_path.c_str(),config.timeout_
#endif
using namespace tinyxml2;
using namespace std;
namespace fs = std::filesystem;
enum Errors{
    ERROR_OK = 0,
    ERROR_UNEXPECTED_NULL_POINTER = 1,
    ERROR_INVALID_OPTION,
    ERROR_INVALID_LOGISIM_OUTPUT
};
constexpr const char* DEFAULT_MARS = "./Mars.jar";
constexpr const char* DEFAULT_CIRCUIT = "CPU.circ";
constexpr const char* DEFAULT_DIR = "test";
constexpr const char* DEFAULT_LOGISIM = "logisim";
struct Configuration{
    bool show_help_ = false;
    string mars_ = DEFAULT_MARS;
    string circuit_ = DEFAULT_CIRCUIT;
    string test_dir_ = DEFAULT_DIR;
    string logisim_ = DEFAULT_LOGISIM;
    unsigned int thread_limit_ = 1;
    unsigned int timeout_ = 5;
};
struct CountInfo{
    private:
        unsigned int total_ = 0;
        unsigned int passed_ = 0;
        unsigned int failed_ = 0;
        unsigned int exception_ = 0;
        unsigned int doubt_ = 0;
        mutex ci_mux_;
    public:
        void pass(){
            lock_guard<mutex> lock(ci_mux_);
            ++total_;
            ++passed_;
        }
        void fail(){
            lock_guard<mutex> lock(ci_mux_);
            ++total_;
            ++failed_;
        }
        void exception(){
            lock_guard<mutex> lock(ci_mux_);
            ++total_;
            ++exception_;
        }
        void doubt(){
            lock_guard<mutex> lock(ci_mux_);
            ++total_;
            ++doubt_;
        }
        double getScore(){
            lock_guard<mutex> lock(ci_mux_);
            return static_cast<double>(passed_) / total_ * 100;
        }
};
class FileAssigner{
    private:
        fs::directory_iterator base_;
        fs::directory_iterator iter_;
        mutex fa_mutex_;
    public:
        FileAssigner(const char* path): base_(path), iter_(begin(base_)){}
        bool get(fs::path& path){
            lock_guard<mutex> lock(fa_mutex_);
            while(iter_ != end(base_) && !((*iter_).is_regular_file())) iter_++;
            if(iter_ == end(base_)) return false;
            path = (*iter_).path();
            iter_++;
            return true;
        }
};
constexpr struct option LONG_CLI_ARGS[] = {
    {"help",            no_argument,        NULL,   'a'},
    {"with-mars",       required_argument,  NULL,   'b'},
    {"test-dir",        required_argument,  NULL,   'c'},
    {"thread",          optional_argument,  NULL,   'd'},
    {"with-logisim",    required_argument,  NULL,   'e'},
    {"with-circuit",    required_argument,  NULL,   'f'},
    {"timeout",         required_argument,  NULL,   'g'},
    {NULL,              0,                  NULL,   0}
};
int getConfiguration(int argc, char** argv, Configuration& config){
    int opt;
    opterr = 0;     // prevent error message
    char* strtol_end = NULL;
    int temp_int;
    while((opt = getopt_long(argc, argv, "", LONG_CLI_ARGS, NULL)) != -1){
        switch(opt){
            case 'a':
                config.show_help_ = true;
                return ERROR_OK;
            case 'b':
                config.mars_ = optarg;
                break;
            case 'c':
                config.test_dir_ = optarg;
                break;
            case 'd':
                if(optarg == NULL){
                    config.thread_limit_ = thread::hardware_concurrency();
                }else{
                    temp_int = strtol(optarg, &strtol_end, 10);
                    if(*strtol_end != '\0') break;
                    config.thread_limit_ = temp_int;
                }
                break;
            case 'e':
                config.logisim_ = optarg;
                break;
            case 'f':
                config.circuit_ = optarg;
                break;
            case 'g':
                temp_int = strtol(optarg, &strtol_end, 10);
                if(*strtol_end != '\0') break;
                config.timeout_ = temp_int;
                break;
            case '?':
                return ERROR_INVALID_OPTION;
        }
    }
    return ERROR_OK;
}
int verifyConfiguration(Configuration& config){
    if(!fs::is_regular_file(config.circuit_)){
        printf("错误: 无法访问的设计文件\n");
        return ERROR_INVALID_OPTION;
    }
    if(!fs::is_directory(config.test_dir_)){
        printf("错误: 指定的测试文件目录无法识别\n");
        return ERROR_INVALID_OPTION;
    }
    if(config.timeout_ <= 0){
        printf("错误: 不合法的超时时间\n");
        return ERROR_INVALID_OPTION;
    }
    return ERROR_OK;
}
void showHelp(){
    printf(
        "LT: 用于Logisim的自动评测机\n"
        "用法: LT [选项(们)]\n"
        "选项:\n"
        "\t--help          显示此消息并退出\n\n"
        "\t--with-mars     指定用于调用Mars的指令\n"
        "\t                请注意使用修改后的Mars来提供需要的信息\n"
        "\t                不提供该选项时的默认值: ./Mars.jar\n\n"
        "\t--test-dir      指定存放测试程序的目录\n"
        "\t                注意LT将不会尝试递归的遍历此目录\n"
        "\t                不提供该选项时的默认值: test\n\n"
        "\t--thread        指定使用的线程数\n"
        "\t                当不指定参数时，采用自动推测的数目\n"
        "\t                不提供该选项时的默认值: 1\n\n"
        "\t--with-logisim  指定用于调用Logisim的指令\n"
        "\t                不提供该选项时的默认值: logisim\n\n"
        "\t--with-circuit  指定您设计的CPU文件\n"
        "\t                不提供该选项时的默认值: CPU.circ\n\n"
        "\t--timeout       指定Logisim仿真的超时时间(单位: 秒)\n"
        "\t                仿真将不会运行超过此时长\n"
        "\t                不提供该选项时的默认值: 5\n\n"
        "\n"
        "如何工作:\n"
        "\t通常而言您仅需要按照上述给出的说明正确指定参数即可使用LT\n"
        "\t无需对您的 CPU 设计作出任何修改\n\n"
        "\t为了避免 PC 溢出导致重复输出带来的误判\n"
        "\t一条跳转到自身的指令将会被自动插入到汇编后的程序末尾\n"
        "\t其编码为 0x1000ffff\n\n"
        "\t通常仿真将在超时后结束\n"
        "\t为了在测试程序运行结束时立即终止仿真，需要对您的CPU做出如下的改动:\n"
        "\t\t在顶层添加一个额外的 1bit 输出并命名为 halt\n"
        "\t\t令当且仅当 WB 段的指令为 0x1000ffff 时其输出有效\n"
        "\t\t且这个端口必须位于所有的其他输出端口的下方\n"
        "\t则当 Logisim 发现名为 halt 的端口有效时将终止仿真\n"
    );
}
namespace TXUtility{
    XMLElement* findContent(XMLElement* entry){
        entry = entry->FirstChildElement();
        while(entry != NULL){
            if(
                !strcmp(entry->Name(), "a") &&
                entry->Attribute("name") != NULL &&
                !strcmp(entry->Attribute("name"), "contents")
            ){
                return entry;
            }
            entry = entry->NextSiblingElement();
        }
        return NULL;
    }
    void searchMemory(XMLElement* entry, char** name){
        while(entry != NULL && *name == NULL){
            if(
                !strcmp(entry->Name(), "lib") &&
                entry->Attribute("desc") != NULL &&
                !strcmp(entry->Attribute("desc"), "#Memory")
            ){
                const char* temp_name = entry->Attribute("name");
                *name = new char[strlen(temp_name)];
                strcpy(*name, temp_name);
                return;
            }
            if(!(entry->NoChildren())) searchMemory(entry->FirstChildElement(), name);
            entry = entry->NextSiblingElement();
        }
    }
    XMLElement* searchROM(XMLElement* entry, const char* name){
        while(entry != NULL){
            if(
                !strcmp(entry->Name(), "comp") &&
                entry->Attribute("lib") != NULL &&
                !strcmp(entry->Attribute("lib"), name) &&
                entry->Attribute("name") != NULL &&
                !strcmp(entry->Attribute("name"), "ROM")
            ){
                return findContent(entry);
            }
            if(!(entry->NoChildren())){
                XMLElement* ret = searchROM(entry->FirstChildElement(), name);
                if(ret != NULL) return ret;
            }
            entry = entry->NextSiblingElement();
        }
        return NULL;
    }
    XMLElement* getROMContext(XMLDocument& doc){
        char* memory_lib_name = NULL;
        XMLElement* entry = doc.FirstChildElement();
        searchMemory(entry, &memory_lib_name);
        entry = doc.FirstChildElement();
        entry = searchROM(entry, memory_lib_name);
        delete[] memory_lib_name;
        return entry;
    }
}
namespace LogisimInterpreter{
    constexpr size_t BUFFER_SIZE = 4096;
    constexpr short ELEMENT_WIDTH[] = {32, 32, 1, 5, 32, 1, 32, 32};
    int interpret(FILE* input, FILE* output){
        char file_buffer[BUFFER_SIZE];
        size_t cur = 0, rare = 0;
        unsigned int values[8] = {0};
        unsigned short element_index = 0, read_width = 0;
        while(true){
            if(cur == rare){
                rare = fread(file_buffer, 1, BUFFER_SIZE, input);
                if(rare == 0) break;
                cur = 0;
            }
            if(file_buffer[cur] == '0' || file_buffer[cur] == '1'){
                if(element_index < 8){
                    values[element_index] = (values[element_index] << 1) | (file_buffer[cur] == '1');
                    ++read_width;
                    if(read_width == ELEMENT_WIDTH[element_index]){
                        read_width = 0;
                        ++element_index;
                        if(element_index == 8){
                            if(values[2]) fprintf(output, "@%08x: $%d <= %08x\n", values[0], values[3], values[4]);
                            if(values[5]) fprintf(output, "@%08x: *%08x <= %08x\n", values[1], values[6], values[7]);
                            for(unsigned int i = 0; i < 8; ++i) values[i] = 0;
                        }
                    }
                }
            }else if(file_buffer[cur] == '\n'){
                if(element_index != 8) return ERROR_INVALID_LOGISIM_OUTPUT;
                element_index = 0;
            }
            ++cur;
        }
        return ERROR_OK;
    }
}
namespace AnswerComparator{
    enum RESULT{
	    PASS = 0,
        FAIL,
	    EXCEPTION,
	    DOUBT
    };
    const regex REGISTER_LINE("^\\s*@(0x)?([0-9a-fA-F]{8})\\s*:\\s*\\$\\s*([0-9]+)\\s*<=\\s*(0x)?([0-9a-fA-F]{8})\\s*$");
    const regex MEMORY_LINE("^\\s*@(0x)?([0-9a-fA-F]{8})\\s*:\\s*\\*\\s*(0x)?([0-9a-fA-F]{8})\\s*<=\\s*(0x)?([0-9a-fA-F]{8})\\s*$");
    constexpr const char* ILLEGALOUTPUT = "遭遇了非法的输出。这可能是您的CPU的错误，但更可能是此程序的问题。";
    constexpr const char* STANDARDOUTPUTILLEGAL = "在解析标准输出时遇到了错误。无法想象。";
    constexpr const char* TOOLONG = "您的输出比标准输出更长。";
    constexpr const char* TOOSHORT = "您的输出比标准输出要短。";
    string getNextLine(istream& log){
    	string temp;
    	smatch m;
    	while(!log.eof()){
    		getline(log, temp);
    		if(!temp.size()) continue;
    		if(regex_match(temp, m, REGISTER_LINE)){
    			if(stoi(m[3]) == 0) continue;
    			temp = "@" + m.str(2) + ": $" + to_string(stoi(m.str(3))) + " <= " + m.str(5);
    			break;
    		}else if(regex_match(temp, m, MEMORY_LINE)){
    			temp = "@" + m.str(2) + ": *" + m.str(4) + " <= " + m.str(6);
    			break;
    		}else{
    			temp = "";
    			break;
    		}
    	}
    	for(char& c: temp) if(c >= 'a' && c <= 'f') c = c - 'a' + 'A';
    	return temp;
    }
    RESULT compare(istream& out, istream& ans, string& comment){
    	string out_line, ans_line;
    	while(!out.eof() && !ans.eof()){
    		out_line = getNextLine(out);
    		ans_line = getNextLine(ans);
    		if(out_line == ans_line) continue;
    		if(!out_line.size()){
    			if(out.eof()) break;
    			comment = ILLEGALOUTPUT;
                return EXCEPTION;
    		}
    		if(!ans_line.size()){
    			if(ans.eof()) break;
    			comment = STANDARDOUTPUTILLEGAL;
                return EXCEPTION;
    		}
            comment = "我们期望 " + out_line + " ，但您的输出是 " + ans_line + " 。";
    		return FAIL;
    	}
    	if(!out.eof()){
            comment = TOOLONG;
            return FAIL;
        }
    	if(!ans.eof()){
            comment = TOOSHORT;
            return FAIL;
        }
        return PASS;
    }
}
void functionalThread(
    FileAssigner& file_assigner,
    const Configuration& config,
    CountInfo& count_info,
    const unsigned int id
){
    char command_buffer[4096];
    char text_buffer[10010] = "addr/data: 10 32\n";
    const size_t text_start = strlen(text_buffer);
    fs::path work_dir = fs::temp_directory_path() / "LT-";
    work_dir += to_string(id);
    fs::create_directories(work_dir);
    fs::path text_path = work_dir / "code.txt";
    fs::path ans_path = work_dir / "ans";
    fs::path temp_out_path = work_dir / "temp_out";
    fs::path out_path = work_dir / "out";
    fs::path circ_path = work_dir / "CPU.circ";
    fs::path test_case;
    XMLDocument xml_doc;
    xml_doc.LoadFile(config.circuit_.c_str());
    XMLElement* content = TXUtility::getROMContext(xml_doc);
    FILE* code_text = NULL;
    FILE* temp_out = NULL;
    FILE* out = NULL;
    size_t read_count;
    ifstream ans;
    ifstream your_ans;
    string comment;
    int ret;
    while(true){
        ret = file_assigner.get(test_case);
        if(!ret) break;
        sprintf(
            command_buffer,
            "%s nc db mc CompactDataAtZero dump .text HexText %s %s > %s",
            config.mars_.c_str(),
            text_path.c_str(),
            test_case.c_str(),
            ans_path.c_str()
        );
        system(command_buffer);
        code_text = fopen(text_path.c_str(), "rb");
        read_count = fread(text_buffer + text_start, 1, 10000 - text_start, code_text);
        fclose(code_text);
        sprintf(text_buffer + text_start + read_count, "\n1000ffff\n00000000");
        content->SetText(text_buffer);
        xml_doc.SaveFile(circ_path.c_str());
        sprintf(command_buffer, LOGISIM_COMMAND);
        system(command_buffer);
        temp_out = fopen(temp_out_path.c_str(), "r");
        out = fopen(out_path.c_str(), "w");
        LogisimInterpreter::interpret(temp_out, out);
        fclose(temp_out);
        fclose(out);
        ans.open(ans_path);
        your_ans.open(out_path);
        ret = AnswerComparator::compare(your_ans, ans, comment);
        if(ret == AnswerComparator::PASS){
            printf("%s通过%s:\t%s\n", GREENCOLOR, ENDCOLOR, test_case.filename().c_str());
            count_info.pass();
        }else if(ret == AnswerComparator::FAIL){
            printf("%s失败%s:\t%s: %s\n", REDCOLOR, ENDCOLOR, test_case.filename().c_str(), comment.c_str());
            count_info.fail();
        }else if(ret == AnswerComparator::EXCEPTION){
            printf("%s异常%s:\t%s: %s\n", PURPLECOLOR, ENDCOLOR, test_case.filename().c_str(), comment.c_str());
            count_info.exception();
        }
    }
    fs::remove_all(work_dir);
}
int main(int argc, char** argv){
    Configuration config;
    if(getConfiguration(argc, argv, config) != ERROR_OK){
        printf("错误: 未识别的选项\n\n");
        showHelp();
        return ERROR_INVALID_OPTION;
    }
    if(config.show_help_){
        showHelp();
        return ERROR_OK;
    }
    if(verifyConfiguration(config) != ERROR_OK) return ERROR_INVALID_OPTION;
    printf(
        "您的设计位于: %s\n测试程序目录位于: %s\n用于调用Mars的命令: %s\n用于调用Logisim的命令: %s\n使用的线程数: %d\n仿真超时为(秒): %d\n",
        config.circuit_.c_str(),
        config.test_dir_.c_str(),
        config.mars_.c_str(),
        config.logisim_.c_str(),
        config.thread_limit_,
        config.timeout_
    );
    vector<thread> threads;
    FileAssigner file_assigner(config.test_dir_.c_str());
    CountInfo count_info;
    srand(time(NULL));
    unsigned int nonce_base = rand();
    for(unsigned int i = 1; i < config.thread_limit_; ++i)
        threads.push_back(thread(functionalThread, ref(file_assigner), ref(config), ref(count_info), nonce_base + i));
    functionalThread(file_assigner, config, count_info, nonce_base + config.thread_limit_);
    for(auto& t: threads) t.join();
    printf("您通过了%lf%%的测试点。\n", count_info.getScore());
    return 0;
}