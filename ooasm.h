#ifndef OOASM_H
#define OOASM_H

#include "computer.h"

Computer::mem_ptr_t mem(const Computer::rval_ptr_t &rv) {
    return std::make_shared<Computer::Mem>(rv);
}

Computer::mov_ptr_t mov(const Computer::lval_ptr_t &dst, const Computer::rval_ptr_t &src) {
    return std::make_shared<Computer::Mov>(dst, src);
}

Computer::num_ptr_t num(int64_t val) {
    return std::make_shared<Computer::Num>(val);
}

Computer::lea_ptr_t lea(const char *id) {
    return std::make_shared<Computer::Lea>(Computer::Id(id));
}

Computer::data_ptr_t data(const char *id, const Computer::num_ptr_t &num) {
    return std::make_shared<Computer::Data>(Computer::Id(id), num);
}

Computer::add_ptr_t add(const Computer::lval_ptr_t &dst, const Computer::rval_ptr_t &val) {
    return std::make_shared<Computer::Add>(dst, val);
}

Computer::sub_ptr_t sub(const Computer::lval_ptr_t &dst, const Computer::rval_ptr_t &val) {
    return std::make_shared<Computer::Sub>(dst, val);
}

Computer::add_ptr_t inc(const Computer::lval_ptr_t &dst) {
    return std::make_shared<Computer::Add>(dst,
                                           std::make_shared<Computer::Num>(1));
}

Computer::sub_ptr_t dec(const Computer::lval_ptr_t &dst) {
    return std::make_shared<Computer::Sub>(dst,
                                           std::make_shared<Computer::Num>(1));
}

Computer::mov_ptr_t one(const Computer::lval_ptr_t &dst) {
    return std::make_shared<Computer::Mov>(dst,
                                           std::make_shared<Computer::Num>(1));
}

Computer::onez_ptr_t onez(const Computer::lval_ptr_t &dst) {
    return std::make_shared<Computer::Onez>(dst);
}

Computer::ones_ptr_t ones(const Computer::lval_ptr_t &dst) {
    return std::make_shared<Computer::Ones>(dst);
}

Computer::Program program(Computer::instructions_t instructions) {
    return Computer::Program(instructions);
}

#endif // OOASM_H
