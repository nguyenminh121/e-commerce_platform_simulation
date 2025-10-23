#include <bits/stdc++.h>
using namespace std;

// ======================
// iShopefy - C++ OOP Demo
// ======================
// Models: Product, Customer, ShoppingCart, Order, Payment
// Interfaces/Abstract: PaymentMethod (CreditCard, PayPal, Cash)
// Polymorphism: DiscountStrategy (Student, Seasonal, Loyalty)

// ---------- Utilities ----------
namespace util {
    inline string nowStr() {
        std::time_t t = std::time(nullptr);
        std::tm tm{};
#ifdef _WIN32
        localtime_s(&tm, &t);
#else
        tm = *std::localtime(&t);
#endif
        char buf[32];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
        return string(buf);
    }

    inline string money(double v) {
        std::ostringstream oss;
        oss.setf(std::ios::fixed); oss<<setprecision(2)<<v;
        return oss.str();
    }
}

// ---------- Product ----------
class Product {
    int id;
    string name;
    double price; // base price
    int stock;
public:
    Product(int id, string name, double price, int stock)
        : id(id), name(std::move(name)), price(price), stock(stock) {}

    int getId() const { return id; }
    const string& getName() const { return name; }
    double getPrice() const { return price; }
    int getStock() const { return stock; }
    void setStock(int s) { stock = s; }
};

// ---------- Customer ----------
class Customer {
    int id;
    string name;
    bool isStudent;
    int loyaltyPoints; // simple metric
public:
    Customer(int id, string name, bool isStudent=false, int loyaltyPoints=0)
        : id(id), name(std::move(name)), isStudent(isStudent), loyaltyPoints(loyaltyPoints) {}
    int getId() const { return id; }
    const string& getName() const { return name; }
    bool getIsStudent() const { return isStudent; }
    int getLoyaltyPoints() const { return loyaltyPoints; }
    void addLoyaltyPoints(int pts) { loyaltyPoints += pts; }
};

// ---------- Discount Strategy (Polymorphism) ----------
class DiscountStrategy {
public:
    virtual ~DiscountStrategy() = default;
    // return discount amount (not final price)
    virtual double computeDiscount(const Customer& c, double subtotal) const = 0;
    virtual string name() const = 0;
};

class NoDiscount : public DiscountStrategy {
public:
    double computeDiscount(const Customer&, double) const override { return 0.0; }
    string name() const override { return "No Discount"; }
};

class StudentDiscount : public DiscountStrategy {
    double rate; // e.g., 10% = 0.10
public:
    explicit StudentDiscount(double rate = 0.10) : rate(rate) {}
    double computeDiscount(const Customer& c, double subtotal) const override {
        return c.getIsStudent() ? subtotal * rate : 0.0;
    }
    string name() const override { return "Student Discount"; }
};

class SeasonalDiscount : public DiscountStrategy {
    double rate;
public:
    explicit SeasonalDiscount(double rate = 0.15) : rate(rate) {}
    double computeDiscount(const Customer&, double subtotal) const override {
        return subtotal * rate; // applied regardless of user
    }
    string name() const override { return "Seasonal Discount"; }
};

class LoyaltyDiscount : public DiscountStrategy {
    int threshold;
    double rate;
public:
    LoyaltyDiscount(int threshold = 100, double rate = 0.05)
        : threshold(threshold), rate(rate) {}
    double computeDiscount(const Customer& c, double subtotal) const override {
        return (c.getLoyaltyPoints() >= threshold) ? subtotal * rate : 0.0;
    }
    string name() const override { return "Loyalty Discount"; }
};

// ---------- Shopping Cart ----------
struct CartItem {
    Product product;
    int quantity;
};

class ShoppingCart {
    vector<CartItem> items;
public:
    void addProduct(const Product& p, int qty=1) {
        for (auto& it : items) {
            if (it.product.getId() == p.getId()) { it.quantity += qty; return; }
        }
        items.push_back({p, qty});
    }
    void removeProduct(int productId) {
        items.erase(remove_if(items.begin(), items.end(), [&](const CartItem& ci){return ci.product.getId()==productId;}), items.end());
    }
    void updateQuantity(int productId, int qty) {
        for (auto& it : items) if (it.product.getId()==productId) { it.quantity = max(0, qty); }
        items.erase(remove_if(items.begin(), items.end(), [](const CartItem& ci){return ci.quantity<=0;}), items.end());
    }
    double subtotal() const {
        double sum = 0.0;
        for (const auto& it : items) sum += it.product.getPrice() * it.quantity;
        return sum;
    }
    const vector<CartItem>& getItems() const { return items; }
    bool empty() const { return items.empty(); }
};

// ---------- Payment Interfaces ----------
class PaymentMethod {
public:
    virtual ~PaymentMethod() = default;
    virtual bool pay(double amount) = 0; // returns success
    virtual string name() const = 0;
};

class CreditCardPayment : public PaymentMethod {
    string cardNumber, holder, expiry;
public:
    CreditCardPayment(string cardNumber, string holder, string expiry)
        : cardNumber(std::move(cardNumber)), holder(std::move(holder)), expiry(std::move(expiry)) {}
    bool pay(double amount) override {
        // Simulate card processing
        cout << "[Payment] Charging Credit Card (\n  Holder: " << holder
             << "\n  Number: ****" << (cardNumber.size()>=4?cardNumber.substr(cardNumber.size()-4):cardNumber)
             << "\n  Expiry: " << expiry << "\n) Amount: $" << util::money(amount) << "\n";
        return true; // assume always success in simulation
    }
    string name() const override { return "Credit Card"; }
};

class PayPalPayment : public PaymentMethod {
    string email;
public:
    explicit PayPalPayment(string email) : email(std::move(email)) {}
    bool pay(double amount) override {
        cout << "[Payment] Processing PayPal (" << email << ") Amount: $" << util::money(amount) << "\n";
        return true;
    }
    string name() const override { return "PayPal"; }
};

class CashPayment : public PaymentMethod {
public:
    bool pay(double amount) override {
        cout << "[Payment] Cash on Delivery Amount Due: $" << util::money(amount) << "\n";
        return true;
    }
    string name() const override { return "Cash"; }
};

// ---------- Order ----------
class Order {
    int orderId;
    Customer customer;
    vector<CartItem> items;
    double subtotalAmount{};
    double discountAmount{};
    double totalAmount{};
    string createdAt;
    string paymentMethodName;
public:
    Order(int orderId, Customer customer, const ShoppingCart& cart)
        : orderId(orderId), customer(std::move(customer)), items(cart.getItems()), createdAt(util::nowStr()) {
        subtotalAmount = 0.0;
        for (const auto& it : items) subtotalAmount += it.product.getPrice() * it.quantity;
    }

    void applyDiscount(const DiscountStrategy& strategy) {
        discountAmount = strategy.computeDiscount(customer, subtotalAmount);
        discountAmount = std::max(0.0, std::min(discountAmount, subtotalAmount));
        totalAmount = subtotalAmount - discountAmount;
    }

    bool processPayment(PaymentMethod& method) {
        paymentMethodName = method.name();
        if (totalAmount <= 0) totalAmount = subtotalAmount; // safety
        return method.pay(totalAmount);
    }

    void printSummary() const {
        cout << "\n===== iShopefy - Order Summary =====\n";
        cout << "Order ID: " << orderId << "\n";
        cout << "Customer: " << customer.getName() << "\n";
        cout << "Created: " << createdAt << "\n";
        cout << "Items:" << "\n";
        for (const auto& it : items) {
            cout << "  - " << it.product.getName() << " x" << it.quantity
                 << " @ $" << util::money(it.product.getPrice())
                 << " = $" << util::money(it.product.getPrice()*it.quantity) << "\n";
        }
        cout << "Subtotal: $" << util::money(subtotalAmount) << "\n";
        cout << "Discount: -$" << util::money(discountAmount) << "\n";
        cout << "Total:    $" << util::money(totalAmount) << "\n";
        cout << "Payment Method: " << paymentMethodName << "\n";
        cout << "====================================\n";
    }
};

// ---------- Demo Main ----------
int main() {
    cout << "Welcome to iShopefy (C++ OOP Simulation)\n";

    // Sample products
    Product p1(1, "Wireless Mouse", 12.50, 50);
    Product p2(2, "Mechanical Keyboard", 45.99, 30);
    Product p3(3, "USB-C Cable", 5.49, 200);

    // Customer
    Customer alice(101, "Alice Nguyen", true /*isStudent*/, 120 /*loyaltyPoints*/);

    // Cart actions
    ShoppingCart cart;
    cart.addProduct(p1, 2);
    cart.addProduct(p2, 1);
    cart.addProduct(p3, 3);
    cart.updateQuantity(3, 2); // change USB-C Cable qty to 2

    cout << "Current cart subtotal: $" << util::money(cart.subtotal()) << "\n";

    // Create order
    Order order(10001, alice, cart);

    // Choose a discount strategy dynamically (polymorphism)
    // We can combine or pick one; here we demonstrate choosing the best among three
    vector<unique_ptr<DiscountStrategy>> strategies;
    strategies.emplace_back(make_unique<StudentDiscount>(0.10));
    strategies.emplace_back(make_unique<SeasonalDiscount>(0.05));
    strategies.emplace_back(make_unique<LoyaltyDiscount>(100, 0.07));

    // Pick the max discount for demonstration
    double bestDisc = 0.0; size_t bestIdx = 0;
    for (size_t i=0; i<strategies.size(); ++i) {
        double d = strategies[i]->computeDiscount(alice, cart.subtotal());
        cout << "Strategy '" << strategies[i]->name() << "' would discount $" << util::money(d) << "\n";
        if (d > bestDisc) { bestDisc = d; bestIdx = i; }
    }
    cout << "Chosen strategy: " << strategies[bestIdx]->name() << "\n";
    order.applyDiscount(*strategies[bestIdx]);

    // Select payment method (simulate user choice)
    // Demonstrate polymorphism across three types
    unique_ptr<PaymentMethod> payment;
    int choice = 1; // 1=CreditCard, 2=PayPal, 3=Cash (hardcoded for demo)
    if (choice == 1) payment = make_unique<CreditCardPayment>("4111111111111111", "Alice Nguyen", "12/27");
    else if (choice == 2) payment = make_unique<PayPalPayment>("alice@example.com");
    else payment = make_unique<CashPayment>();

    // Process payment via order and persist method name
    bool success = order.processPayment(*payment);
    if (!success) {
        cerr << "Payment failed. Please try another method." << endl;
        return 1;
    }
    order.printSummary();

    // Post-order loyalty
    int pointsEarned = static_cast<int>(max(1.0, (cart.subtotal() - bestDisc) / 10.0));
    cout << "Loyalty points earned: " << pointsEarned << "\n";

    cout << "Thank you for shopping at iShopefy!\n";
    return 0;
}
