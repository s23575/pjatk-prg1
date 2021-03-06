/*
 * A reverse-polish notation calculator.
 */

#include <s23575/RPN_calculator.h>

#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <iterator>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

static auto pop_top(std::stack<double>& stack) -> double
{
    if (stack.empty()) {
        throw std::logic_error{"empty stack"};
    }
    auto element = std::move(stack.top());
    stack.pop();
    return element;
}

namespace s23575 { namespace rpn_calculator {
Element::~Element()
{}

Literal::Literal(double const v) : value{v}
{}
auto Literal::evaluate(stack_type& stack) const -> void
{
    stack.push(value);
}

auto Print::evaluate(stack_type& stack) const -> void
{
    std::cout << stack.top() << "\n";
}

auto evaluate_binary_op(stack_type& stack,
                        std::function<double(double, double)> op,
                        std::string op_name) -> void
{
    if (stack.size() < 2) {
        throw std::logic_error{"not enough operands for " + op_name};
    }

    auto const b = pop_top(stack);
    auto const a = pop_top(stack);

    stack.push(op(a, b));
}

auto evaluate_binary_op(stack_type& stack,
                        std::function<double(double)> op,
                        std::string op_name) -> void
{
    if (stack.size() < 1) {
        throw std::logic_error{"not enough operands for " + op_name};
    }

    auto const a = pop_top(stack);

    stack.push(op(a));
}

auto Addition::evaluate(stack_type& stack) const -> void
{
    evaluate_binary_op(stack, std::plus<>(), "+");
}

auto Subtraction::evaluate(stack_type& stack) const -> void
{
    evaluate_binary_op(stack, std::minus<>(), "-");
}

auto Multiplication::evaluate(stack_type& stack) const -> void
{
    evaluate_binary_op(stack, std::multiplies<>(), "*");
}

auto Division::evaluate(stack_type& stack) const -> void
{
    evaluate_binary_op(stack, std::divides<>(), "/");
}


// Nie jestem pewien, który zapis dodatkowej funkcji jest lepszy/preferowany -
// poniższy, z odrębną definicją pożądanej funkcji, czy zastosowany dalej, przy
// wykorzystaniu std::function.

/*
auto div_int(double a, double b) -> double
{
    auto c = static_cast<int>(a / b);
    return c;
}
*/

auto Division_Integers::evaluate(stack_type& stack) const -> void
{
    std::function<double(double, double)> div_int = [](double a, double b) {
        return static_cast<int>(a / b);
    };
    evaluate_binary_op(stack, div_int, "//");
}

auto Reminder::evaluate(stack_type& stack) const -> void
{
    evaluate_binary_op(stack, fmod, "%");
}

auto Exponentiation::evaluate(stack_type& stack) const -> void
{
    evaluate_binary_op(stack, pow, "**");
}

auto Square_Root::evaluate(stack_type& stack) const -> void
{
    evaluate_binary_op(stack, sqrt, "sqrt");
}

/*
auto sec_to_h(double a) -> double
{
    auto c = a / 3600;
    return c;
}
*/

auto Seconds_To_Hours::evaluate(stack_type& stack) const -> void
{
    std::function<double(double)> sec_to_h = [](double a) { return a / 3600; };
    evaluate_binary_op(stack, sec_to_h, "h");
}

Calculator::Calculator(stack_type s) : stack{std::move(s)}
{}

auto Calculator::push(std::unique_ptr<Element> op) -> void
{
    ops.push(std::move(op));
}

// FIXME implement Calculator::push(std::string)

auto Calculator::evaluate() -> void
{
    while (not ops.empty()) {
        auto op = std::move(ops.front());
        ops.pop();

        op->evaluate(stack);
    }
}
}}  // namespace s23575::rpn_calculator


auto make_args(int argc, char* argv[], bool const with_exec = false)
    -> std::vector<std::string>
{
    auto args         = std::vector<std::string>{};
    auto const offset = static_cast<size_t>(not with_exec);
    std::copy_n(argv + offset, argc - offset, std::back_inserter(args));
    return args;
}

auto main(int argc, char* argv[]) -> int
{
    using s23575::rpn_calculator::Calculator;
    auto calculator = Calculator{};

    for (auto const& each : make_args(argc, argv)) {
        try {
            using s23575::rpn_calculator::Addition;
            using s23575::rpn_calculator::Division;
            using s23575::rpn_calculator::Division_Integers;
            using s23575::rpn_calculator::Exponentiation;
            using s23575::rpn_calculator::Literal;
            using s23575::rpn_calculator::Multiplication;
            using s23575::rpn_calculator::Print;
            using s23575::rpn_calculator::Reminder;
            using s23575::rpn_calculator::Seconds_To_Hours;
            using s23575::rpn_calculator::Square_Root;
            using s23575::rpn_calculator::Subtraction;

            if (each == "p") {
                calculator.push(std::make_unique<Print>());
            } else if (each == "+") {
                calculator.push(std::make_unique<Addition>());
            } else if (each == "-") {
                calculator.push(std::make_unique<Subtraction>());
            } else if (each == "*") {
                calculator.push(std::make_unique<Multiplication>());
            } else if (each == "/") {
                calculator.push(std::make_unique<Division>());
            } else if (each == "//") {
                calculator.push(std::make_unique<Division_Integers>());
            } else if (each == "%") {
                calculator.push(std::make_unique<Reminder>());
            } else if (each == "**") {
                calculator.push(std::make_unique<Exponentiation>());
            } else if (each == "sqrt") {
                calculator.push(std::make_unique<Square_Root>());
            } else if (each == "h") {
                calculator.push(std::make_unique<Seconds_To_Hours>());
            } else {
                calculator.push(std::make_unique<Literal>(std::stod(each)));
            }
        } catch (std::logic_error const& e) {
            std::cerr << "error: " << each << ": " << e.what() << "\n";
        }
    }

    try {
        calculator.evaluate();
    } catch (std::logic_error const& e) {
        std::cerr << "error: during evaluation: " << e.what() << "\n";
    }

    return 0;
}
