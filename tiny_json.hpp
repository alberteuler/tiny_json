
#ifndef TINYJSON_H__
#define TINYJSON_H__

#include <string>
#include <assert.h>
typedef enum {
    TINY_NULL,
    TINY_FALSE,
    TINY_TRUE,
    TINY_NUMBER,
    TINY_STRING,
    TINY_ARRAY,
    TINY_OBJ
} tiny_type;

typedef enum{
    STATUS_OK,
    STATUS_EXPECT_VAL,
    STATUS_INVALD_VALUE,
    STATUS_ROOT_NOT_SINGULAR,
    STATUS_MISS_QUTATION_MARK,
    STATUS_INVALID_UNICODE
} status;

typedef struct {
    char* json;
}context;

typedef struct {
    union {
        double number;
        bool boolean;
        int* integer;
        std::string string;
    }u;
    tiny_type type;
}value;

class Parse {
private:
    context* main_ptr;
    context* c;
    int status;
    value* v;
    void get_whitespace();
    int get_null( );
    int get_false( );
    int get_true( );
    int get_number( );
    int get_string();
    int encode_utf8();
public:
    Parse():
        main_ptr(nullptr),
        status(STATUS_OK)
        {};
    Parse(context *__c):
        status(STATUS_OK),
        c(__c){
            main_ptr = __c;
        }
    int get_value();
    char* get_ptr() const;
    int get_status() const;
    value* get_val() const { return v;}
};

#define EXPECT(c, ch) do { assert(* main_ptr->json == (ch));  main_ptr->json++; } while(0)
#define ISDIGIT(ch) ((ch) >= '0' && (ch) <= '9')
#define IS129(ch) ((ch) >= '1' && (ch) <= '9')
char* Parse::get_ptr() const{
    return main_ptr->json;
}


void Parse::get_whitespace() {
    while(*main_ptr->json == ' ' || *main_ptr->json == '\t' || *main_ptr->json == '\n' || *main_ptr->json == '\r') main_ptr->json++;
}

int Parse::get_null( ) {
    EXPECT(c, 'n');
    if( main_ptr->json[0] != 'u' ||  main_ptr->json[1] != 'l' || main_ptr->json[2] != 'l') return STATUS_INVALD_VALUE;
     main_ptr->json += 3;
    main_ptr += 3;
    v->type = TINY_NULL;
    return STATUS_OK;
}

int Parse::get_false( ) {
    EXPECT(c, 'f');
    if( main_ptr->json[0] != 'a' || main_ptr->json[1] != 'l' || main_ptr->json[2] != 's' ||  main_ptr->json[3] != 'e') return STATUS_INVALD_VALUE;
     main_ptr->json += 4;
    main_ptr += 4;
    v->type = TINY_FALSE;
    return STATUS_OK;
}

int Parse::get_true( ) {
    EXPECT(c, 't');
    if( main_ptr->json[0] != 'r' ||  main_ptr->json[1] != 'u' || main_ptr->json[2] != 'e') return STATUS_INVALD_VALUE;
     main_ptr->json += 3;
    main_ptr += 3;
    v->type = TINY_TRUE;
    return STATUS_OK;
}

int Parse::get_value( ) {
    switch(*main_ptr->json) {
        case 'n' : return get_null();
        case 't' : return get_true();
        case 'f' : return get_false();
        case '\0' : return STATUS_EXPECT_VAL;
        default:
            if(ISDIGIT(*main_ptr->json)) {
                return get_number();
            }else return get_string();
        
    }
}

int Parse::get_number() {
    char *endptr;
    v->u.number = strtod(main_ptr->json, &endptr);
    if(main_ptr->json == endptr) return STATUS_INVALD_VALUE;
    main_ptr->json = endptr;
    v->type = TINY_NUMBER;
    return STATUS_OK;
}

int Parse::get_string(){
    char *begin = main_ptr->json;
    char *ptr = begin;
    while(1) {
        ptr ++;
        switch( *ptr) {
            case '\"': 
                int len = ptr - begin;
                std::string &&str = std::move(std::string(begin, len));
                v->u.string = str;
                return STATUS_OK;
            case '\0': 
                return STATUS_MISS_QUTATION_MARK;
            default: continue;
        }
    }
}

char utf8(const char *str) {
    if(*str >= 0x0800 && *str <= 0xFFFF) {
        return 0XE0 | ((*str) >> 12 &0XFF);
    }
}

//TODO: 完成utf-8的解码工作。
int Parse::encode_utf8(){
    
}
#endif