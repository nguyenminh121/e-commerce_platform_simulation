#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <map>
#include <algorithm>
#include <cctype>

using namespace std;

// Simple JSON parser (minimal implementation)
class SimpleJSON {
public:
    map<string, string> data;
    
    void parseFromString(const string& jsonStr) {
        // Very basic JSON parsing for demo purposes
        // In real application, use proper JSON library
        data.clear();
        
        // Extract key-value pairs (simplified)
        size_t pos = 0;
        while ((pos = jsonStr.find("\"", pos)) != string::npos) {
            size_t keyStart = pos + 1;
            size_t keyEnd = jsonStr.find("\"", keyStart);
            if (keyEnd == string::npos) break;
            
            string key = jsonStr.substr(keyStart, keyEnd - keyStart);
            
            pos = jsonStr.find(":", keyEnd);
            if (pos == string::npos) break;
            
            pos = jsonStr.find("\"", pos);
            if (pos == string::npos) break;
            
            size_t valueStart = pos + 1;
            size_t valueEnd = jsonStr.find("\"", valueStart);
            if (valueEnd == string::npos) break;
            
            string value = jsonStr.substr(valueStart, valueEnd - valueStart);
            data[key] = value;
            
            pos = valueEnd + 1;
        }
    }
    
    string get(const string& key, const string& defaultValue = "") const {
        auto it = data.find(key);
        return (it != data.end()) ? it->second : defaultValue;
    }
    
    string toString() {
        string result = "{";
        bool first = true;
        for (const auto& pair : data) {
            if (!first) result += ",";
            result += "\"" + pair.first + "\":\"" + pair.second + "\"";
            first = false;
        }
        result += "}";
        return result;
    }
};

// File-based communication with PHP (simple bridge)
class PHPBridge {
private:
    string dataDir;
    
public:
    PHPBridge() : dataDir("data/") {
        // Create data directory if it doesn't exist
        #ifdef _WIN32
            system("if not exist data mkdir data");
        #else
            system("mkdir -p data");
        #endif
    }
    
    bool callPHP(const string& script, const string& input = "", string* output = nullptr) {
        // Write input to file
        string inputFile = dataDir + "input.json";
        string outputFile = dataDir + "output.json";
        
        if (!input.empty()) {
            ofstream inFile(inputFile);
            inFile << input;
            inFile.close();
        }
        
        // Call PHP script
        string command = "php " + script + " > " + outputFile + " 2>&1";
        int result = system(command.c_str());
        
        // Read output
        if (output != nullptr) {
            ifstream outFile(outputFile);
            if (outFile.is_open()) {
                ostringstream buffer;
                buffer << outFile.rdbuf();
                *output = buffer.str();
                outFile.close();
                return result == 0;
            }
        }
        
        return false;
    }
};

// Product class
class Product {
public:
    int id;
    string name;
    double price;
    int stock;
    string category;
    string description;
    
    Product(int id = -1, const string& name = "", double price = 0, int stock = 0, 
           const string& category = "", const string& description = "")
        : id(id), name(name), price(price), stock(stock), 
          category(category), description(description) {}
    
    // Create from SimpleJSON
    static Product fromJSON(const SimpleJSON& json) {
        return Product(
            stoi(json.get("id", "-1")),
            json.get("name", ""),
            stod(json.get("price", "0")),
            stoi(json.get("stock", "0")),
            json.get("category", ""),
            json.get("description", "")
        );
    }
    
    // Convert to SimpleJSON string
    string toJSONString() const {
        return "{\"id\":\"" + to_string(id) + 
               "\",\"name\":\"" + name + 
               "\",\"price\":\"" + to_string(price) + 
               "\",\"stock\":\"" + to_string(stock) + 
               "\",\"category\":\"" + category + 
               "\",\"description\":\"" + description + "\"}";
    }
    
    void display() const {
        cout << "ID: " << id << "\n"
             << "Name: " << name << "\n"
             << "Price: " << fixed << setprecision(0) << price << " VND\n"
             << "Stock: " << stock << "\n"
             << "Category: " << category << "\n"
             << "Description: " << description << "\n"
             << "------------------------\n";
    }
    
    bool isValid() const {
        return id != -1 && !name.empty();
    }
};

// API Client for iShopefy (using file-based communication)
class iShopefyAPI {
private:
    PHPBridge bridge;
    vector<Product> mockProducts;
    
    void initMockData() {
        mockProducts.clear();
        mockProducts.push_back(Product(1, "iPhone 15 Pro Max", 28990000, 10, "Electronics", "Latest iPhone with 256GB storage"));
        mockProducts.push_back(Product(2, "Dell XPS 13", 22490000, 5, "Electronics", "Ultrabook with Intel i7"));
        mockProducts.push_back(Product(3, "Sony WH-1000XM5", 5990000, 20, "Electronics", "Noise cancelling headphones"));
        mockProducts.push_back(Product(4, "Apple Watch Series 9", 8490000, 15, "Electronics", "Smartwatch with health tracking"));
        mockProducts.push_back(Product(5, "iPad Air M2", 15990000, 8, "Electronics", "Tablet with 256GB storage"));
    }
    
public:
    iShopefyAPI() {
        initMockData();
    }
    
    // Get all products (mock data for demonstration)
    vector<Product> getProducts() {
        cout << "[API] Fetching products from mock database...\n";
        return mockProducts;
    }
    
    // Get single product
    Product getProduct(int id) {
        cout << "[API] Fetching product ID: " << id << "\n";
        for(const auto& product : mockProducts) {
            if(product.id == id) {
                return product;
            }
        }
        return Product(); // Return invalid product
    }
    
    // Add new product
    bool addProduct(const Product& product) {
        cout << "[API] Adding new product: " << product.name << "\n";
        
        // Find next available ID
        int nextId = 1;
        for(const auto& p : mockProducts) {
            if(p.id >= nextId) {
                nextId = p.id + 1;
            }
        }
        
        Product newProduct = product;
        newProduct.id = nextId;
        mockProducts.push_back(newProduct);
        
        cout << "[API] Product added successfully with ID: " << nextId << "\n";
        return true;
    }
    
    // Update product stock
    bool updateStock(int productId, int newStock) {
        cout << "[API] Updating stock for product ID: " << productId << "\n";
        for(auto& product : mockProducts) {
            if(product.id == productId) {
                product.stock = newStock;
                cout << "[API] Stock updated successfully\n";
                return true;
            }
        }
        cout << "[API] Product not found\n";
        return false;
    }
    
    // Search products by name
    vector<Product> searchProducts(const string& query) {
        cout << "[API] Searching products for: " << query << "\n";
        vector<Product> results;
        
        string lowerQuery = query;
        transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
        
        for(const auto& product : mockProducts) {
            string lowerName = product.name;
            transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
            
            if(lowerName.find(lowerQuery) != string::npos) {
                results.push_back(product);
            }
        }
        
        return results;
    }
};

// Demo application
class iShopefyConsole {
private:
    iShopefyAPI api;
    
public:
    void run() {
        cout << "=================================\n";
        cout << "  iShopefy C++ Web Integration\n";
        cout << "=================================\n\n";
        
        int choice;
        do {
            cout << "\n=== iShopefy Management System ===\n";
            cout << "1. View All Products\n";
            cout << "2. View Single Product\n";
            cout << "3. Add New Product\n";
            cout << "4. Update Product Stock\n";
            cout << "5. Search Products\n";
            cout << "6. Exit\n";
            cout << "Choose option (1-6): ";
            cin >> choice;
            
            switch(choice) {
                case 1: viewAllProducts(); break;
                case 2: viewSingleProduct(); break;
                case 3: addNewProduct(); break;
                case 4: updateProductStock(); break;
                case 5: searchProducts(); break;
                case 6: cout << "Thank you for using iShopefy!\n"; break;
                default: cout << "Invalid choice! Please select 1-6.\n";
            }
            
            if(choice != 6) {
                cout << "\nPress Enter to continue...";
                cin.ignore();
                cin.get();
            }
        } while(choice != 6);
    }
    
private:
    void viewAllProducts() {
        cout << "\n--- Fetching products from web API ---\n";
        auto products = api.getProducts();
        
        if(products.empty()) {
            cout << "No products found or API error.\n";
            return;
        }
        
        cout << "\nFound " << products.size() << " products:\n\n";
        for(const auto& product : products) {
            product.display();
        }
    }
    
    void viewSingleProduct() {
        int id;
        cout << "Enter product ID: ";
        cin >> id;
        
        auto product = api.getProduct(id);
        if(product.id == -1) {
            cout << "Product not found!\n";
        } else {
            cout << "\nProduct Details:\n";
            product.display();
        }
    }
    
    void addNewProduct() {
        string name, category, description;
        double price;
        int stock;
        
        cin.ignore(); // Clear input buffer
        cout << "\n--- Add New Product ---\n";
        cout << "Enter product name: ";
        getline(cin, name);
        cout << "Enter price (VND): ";
        cin >> price;
        cout << "Enter stock quantity: ";
        cin >> stock;
        cin.ignore();
        cout << "Enter category: ";
        getline(cin, category);
        cout << "Enter description: ";
        getline(cin, description);
        
        Product newProduct(0, name, price, stock, category, description);
        
        if(api.addProduct(newProduct)) {
            cout << "\n✅ Product added successfully!\n";
        } else {
            cout << "\n❌ Failed to add product!\n";
        }
    }
    
    void updateProductStock() {
        int id, newStock;
        cout << "\n--- Update Product Stock ---\n";
        cout << "Enter product ID: ";
        cin >> id;
        cout << "Enter new stock quantity: ";
        cin >> newStock;
        
        if(api.updateStock(id, newStock)) {
            cout << "\n✅ Stock updated successfully!\n";
        } else {
            cout << "\n❌ Failed to update stock! Product not found.\n";
        }
    }
    
    void searchProducts() {
        string query;
        cin.ignore(); // Clear input buffer
        cout << "\n--- Search Products ---\n";
        cout << "Enter search term: ";
        getline(cin, query);
        
        auto results = api.searchProducts(query);
        if(results.empty()) {
            cout << "\nNo products found matching: " << query << "\n";
        } else {
            cout << "\nFound " << results.size() << " product(s):\n\n";
            for(const auto& product : results) {
                product.display();
            }
        }
    }
};

int main() {
    try {
        iShopefyConsole app;
        app.run();
    } catch(const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}