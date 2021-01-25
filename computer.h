#ifndef COMPUTER_H
#define COMPUTER_H

#include <iostream>
#include <vector>
#include <map>
#include <memory>


class Computer {
public:
    class ElementASM {
    protected:
        Computer *computer;
    public:
        ElementASM() : computer(nullptr) {}

        virtual ~ElementASM() = default;

        ElementASM(const ElementASM &other) = default;

        ElementASM &operator=(const ElementASM &other) = default;

        ElementASM(ElementASM &&other) = default;

        ElementASM &operator=(ElementASM &&other) = default;

        virtual void set_computer(Computer *new_computer) {
            computer = new_computer;
        }

        virtual void execute() = 0;

        virtual ElementASM *clone() = 0;
    };


    class Id : public ElementASM {
    private:
        std::string id;

        static const size_t min_id_len = 1;
        static const size_t max_id_len = 10;
    public:
        explicit Id(const char *id) : id(id) {
            if (this->id.length() < min_id_len || this->id.length() > max_id_len) {
                throw std::invalid_argument("Invalid identifier");
            }
        }

        ~Id() override = default;

        Id(const Id &other) = default;

        Id &operator=(const Id &other) = default;

        Id(Id &&other) = default;

        Id &operator=(Id &&other) = default;

        void execute() override {
        }

        Id *clone() override {
            return new Id(*this);
        }

        [[nodiscard]] const std::string &get_id() const {
            return id;
        }

        bool operator<(const Id &other) const noexcept {
            return id < other.id;
        }
    };


    class RValue : public ElementASM {
    protected:
        int64_t value;
    public:
        RValue() : value(0) {}

        explicit RValue(int64_t value) : value(value) {}

        ~RValue() override = default;

        RValue(const RValue &other) = default;

        RValue &operator=(const RValue &other) = default;

        RValue(RValue &&other) = default;

        RValue &operator=(RValue &&other) = default;

        [[nodiscard]] int64_t get_value() const {
            return value;
        }
    };


    using rval_ptr_t = std::shared_ptr<RValue>;


    class LValue : public RValue {
    protected:
        uint64_t address;
    public:
        LValue() : address(0) {}

        ~LValue() override = default;

        LValue(const LValue &other) = default;

        LValue &operator=(const LValue &other) = default;

        LValue(LValue &&other) = default;

        LValue &operator=(LValue &&other) = default;

        [[nodiscard]] uint64_t get_address() const {
            return address;
        }
    };


    using lval_ptr_t = std::shared_ptr<LValue>;


    class Num : public RValue {
    public:
        explicit Num(int64_t value) : RValue(value) {}

        ~Num() override = default;

        Num(const Num &other) = default;

        Num &operator=(const Num &other) = default;

        Num(Num &&other) = default;

        Num &operator=(Num &&other) = default;

        void execute() override {
        }

        Num *clone() override {
            return new Num(*this);
        }
    };


    using num_ptr_t = std::shared_ptr<Computer::Num>;


    class Lea : public RValue {
    private:
        Id id;
    public:
        explicit Lea(Id id) : id(std::move(id)) {}

        ~Lea() override = default;

        Lea(const Lea &other) = default;

        Lea &operator=(const Lea &other) = default;

        Lea(Lea &&other) = default;

        Lea &operator=(Lea &&other) = default;

        void execute() override {
            value = computer->memory[id];
        }

        Lea *clone() override {
            return new Lea(*this);
        }
    };


    using lea_ptr_t = std::shared_ptr<Computer::Lea>;


    class Mem : public LValue {
    private:
        rval_ptr_t input;
    public:
        explicit Mem(rval_ptr_t input) : input(std::move(input)) {}

        ~Mem() override = default;

        Mem(const Mem &other) = default;

        Mem &operator=(const Mem &other) = default;

        Mem(Mem &&other) = default;

        Mem &operator=(Mem &&other) = default;

        void execute() override {
            input->execute();
            int64_t comp_address = input->get_value();

            value = computer->memory[comp_address];
            address = comp_address;
        }

        void set_computer(Computer *computer) override {
            LValue::set_computer(computer);
            input->set_computer(computer);
        }

        Mem *clone() override {
            return new Mem(*this);
        }
    };


    using mem_ptr_t = std::shared_ptr<Computer::Mem>;


    class Instruction : public ElementASM {
    private:
        bool exec_first;
    public:
        Instruction() : exec_first(false) {}

        explicit Instruction(bool execute_first) : exec_first(execute_first) {}

        ~Instruction() override = default;

        Instruction(const Instruction &other) = default;

        Instruction &operator=(const Instruction &other) = default;

        Instruction(Instruction &&other) = default;

        Instruction &operator=(Instruction &&other) = default;

        [[nodiscard]] bool execute_first() const {
            return exec_first;
        }
    };


    using instr_ptr_t = std::shared_ptr<Computer::Instruction>;
    using instructions_t = std::initializer_list<instr_ptr_t>;


    class Mov : public Instruction {
    private:
        lval_ptr_t dst;
        rval_ptr_t src;
    public:
        Mov(lval_ptr_t dst, rval_ptr_t src) : dst(std::move(dst)),
                                              src(std::move(src)) {}

        ~Mov() override = default;

        Mov(const Mov &other) = default;

        Mov &operator=(const Mov &other) = default;

        Mov(Mov &&other) = default;

        Mov &operator=(Mov &&other) = default;

        void execute() override {
            dst->execute();
            src->execute();
            uint64_t dst_address = dst->get_address();
            int64_t src_value = src->get_value();

            computer->memory.set_value(dst_address, src_value);
        }

        void set_computer(Computer *computer) override {
            Instruction::set_computer(computer);
            dst->set_computer(computer);
            src->set_computer(computer);
        }

        Mov *clone() override {
            return new Mov(*this);
        }
    };


    using mov_ptr_t = std::shared_ptr<Computer::Mov>;


    class Data : public Instruction {
    private:
        Id id;
        num_ptr_t num;
    public:
        Data(Id id, num_ptr_t num) : Instruction(true),
                                     id(std::move(id)), num(std::move(num)) {}

        ~Data() override = default;

        Data(const Data &other) = default;

        Data &operator=(const Data &other) = default;

        Data(Data &&other) = default;

        Data &operator=(Data &&other) = default;

        void execute() override {
            computer->memory.define_data(id, num->get_value());
        }

        void set_computer(Computer *computer) override {
            Instruction::set_computer(computer);
            num->set_computer(computer);
        }

        Data *clone() override {
            return new Data(*this);
        }
    };


    using data_ptr_t = std::shared_ptr<Computer::Data>;


    class ArithmeticOperation : public Instruction {
    private:
        using operation_t = int64_t (*)(int64_t, int64_t);

        lval_ptr_t dst;
        rval_ptr_t val;
        operation_t operation;
    public:
        ArithmeticOperation(lval_ptr_t dst, rval_ptr_t val,
                            operation_t operation) : dst(std::move(dst)),
                                                     val(std::move(val)),
                                                     operation(operation) {}

        ~ArithmeticOperation() override = default;

        ArithmeticOperation(const ArithmeticOperation &other) = default;

        ArithmeticOperation &operator=(const ArithmeticOperation &other) = default;

        ArithmeticOperation(ArithmeticOperation &&other) = default;

        ArithmeticOperation &operator=(ArithmeticOperation &&other) = default;

        void execute() override {
            dst->execute();
            val->execute();
            uint64_t dst_address = dst->get_address();
            int64_t value = val->get_value();

            int64_t old_value = computer->memory[dst_address];
            int64_t new_value = operation(old_value, value);
            computer->memory.set_value(dst_address, new_value);

            computer->processor.set_zero_flag(new_value == 0);
            computer->processor.set_sign_flag(new_value < 0);
        }

        void set_computer(Computer *computer) override {
            Instruction::set_computer(computer);
            dst->set_computer(computer);
            val->set_computer(computer);
        }

        ArithmeticOperation *clone() override {
            return new ArithmeticOperation(*this);
        }
    };


    class Add : public ArithmeticOperation {
    public:
        Add(lval_ptr_t dst, rval_ptr_t val) :
                ArithmeticOperation(std::move(dst), std::move(val),
                                    [](int64_t val_1, int64_t val_2) {
                                        return val_1 + val_2;
                                    }) {}

        ~Add() override = default;

        Add(const Add &other) = default;

        Add &operator=(const Add &other) = default;

        Add(Add &&other) = default;

        Add &operator=(Add &&other) = default;

        Add *clone() override {
            return new Add(*this);
        }
    };


    using add_ptr_t = std::shared_ptr<Computer::Add>;


    class Sub : public ArithmeticOperation {
    public:
        Sub(lval_ptr_t dst, rval_ptr_t val) :
                ArithmeticOperation(std::move(dst), std::move(val),
                                    [](int64_t val_1, int64_t val_2) {
                                        return val_1 - val_2;
                                    }) {}

        ~Sub() override = default;

        Sub(const Sub &other) = default;

        Sub &operator=(const Sub &other) = default;

        Sub(Sub &&other) = default;

        Sub &operator=(Sub &&other) = default;

        Sub *clone() override {
            return new Sub(*this);
        }
    };


    using sub_ptr_t = std::shared_ptr<Computer::Sub>;


    class Onez : public Mov {
    public:
        explicit Onez(lval_ptr_t dst) : Mov(std::move(dst),
                                            std::make_shared<Num>(1)) {}

        ~Onez() override = default;

        Onez(const Onez &other) = default;

        Onez &operator=(const Onez &other) = default;

        Onez(Onez &&other) = default;

        Onez &operator=(Onez &&other) = default;

        void execute() override {
            if (computer->processor.zero_flag_set()) {
                Mov::execute();
            }
        }

        Onez *clone() override {
            return new Onez(*this);
        }
    };


    using onez_ptr_t = std::shared_ptr<Computer::Onez>;


    class Ones : public Mov {
    public:
        explicit Ones(lval_ptr_t dst) : Mov(std::move(dst),
                                            std::make_shared<Num>(1)) {}

        ~Ones() override = default;

        Ones(const Ones &other) = default;

        Ones &operator=(const Ones &other) = default;

        Ones(Ones &&other) = default;

        Ones &operator=(Ones &&other) = default;

        void execute() override {
            if (computer->processor.sign_flag_set()) {
                Mov::execute();
            }
        }

        Ones *clone() override {
            return new Ones(*this);
        }
    };


    using ones_ptr_t = std::shared_ptr<Computer::Ones>;


    class Processor {
    private:
        bool zero_flag;
        bool sign_flag;
    public:
        Processor() : zero_flag(false), sign_flag(false) {}

        ~Processor() = default;

        Processor(const Processor &other) = default;

        Processor &operator=(const Processor &other) = default;

        Processor(Processor &&other) = default;

        Processor &operator=(Processor &&other) = default;

        [[nodiscard]] bool zero_flag_set() const {
            return zero_flag;
        }

        void set_zero_flag(bool new_value) {
            zero_flag = new_value;
        }

        [[nodiscard]] bool sign_flag_set() const {
            return sign_flag;
        }

        void set_sign_flag(bool new_value) {
            sign_flag = new_value;
        }
    };


    class Memory {
    private:
        using memory_t = std::vector<int64_t>;
        using label_t = std::map<Id, int64_t>;

        memory_t memory;
        uint64_t memory_size;
        label_t id_to_address;

        void check_if_valid_address(uint64_t address) const {
            if (memory.size() <= address) {
                throw std::out_of_range("Invalid memory address");
            }
        }

    public:
        explicit Memory(uint64_t memory_size) : memory_size(memory_size) {}

        ~Memory() = default;

        Memory(const Memory &other) = default;

        Memory &operator=(const Memory &other) = default;

        Memory(Memory &&other) = default;

        Memory &operator=(Memory &&other) = default;

        void init() {
            memory.assign(memory_size, 0);
            id_to_address.clear();
        }

        uint64_t operator[](const Id &id) const {
            auto it = id_to_address.find(id);
            if (it == id_to_address.end()) {
                throw std::invalid_argument("Undefined identifier");
            }

            return it->second;
        }

        const int64_t &operator[](uint64_t address) const {
            check_if_valid_address(address);

            return memory[address];
        }

        void set_value(uint64_t address, int64_t new_value) {
            check_if_valid_address(address);

            memory[address] = new_value;
        }

        void define_data(const Id &id, int64_t value) {
            size_t new_address = id_to_address.size();
            if (new_address == memory.size()) {
                throw std::out_of_range("Memory size exceeded");
            }

            memory[new_address] = value;
            if (id_to_address.find(id) == id_to_address.end()) {
                id_to_address[id] = new_address;
            }
        }

        [[nodiscard]] const memory_t &get_memory() const {
            return memory;
        }
    };


    class Program {
    private:
        using instr_vec_t = std::vector<instr_ptr_t>;

        instr_vec_t instructions;
    public:
        Program(instructions_t instructions) : instructions(instructions) {}

        ~Program() = default;

        Program(const Program &other) = default;

        Program &operator=(const Program &other) = default;

        Program(Program &&other) = default;

        Program &operator=(Program &&other) = default;

        [[nodiscard]] const instr_vec_t &get_instructions() const {
            return instructions;
        }
    };


    explicit Computer(uint64_t memory_size) : memory(memory_size) {}

    ~Computer() = default;

    Computer(const Computer &other) = default;

    Computer &operator=(const Computer &other) = default;

    Computer(Computer &&other) = default;

    Computer &operator=(Computer &&other) = default;

    void boot(const Program &program) {
        memory.init();

        for (auto &instr: program.get_instructions()) {
            instr->set_computer(this);
            if (instr->execute_first()) {
                instr->execute();
            }
        }

        for (auto &instr: program.get_instructions()) {
            if (!instr->execute_first()) {
                instr->execute();
            }
        }
    }

    void memory_dump(std::ostream &stream) const {
        for (auto &elem: memory.get_memory()) {
            stream << elem << " ";
        }
    }

private:
    Processor processor;
    Memory memory;
};


#endif // COMPUTER_H
