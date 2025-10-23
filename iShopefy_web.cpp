#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

// HTTP Response structure
struct HTTPResponse {
    long status_code;
    string body;
    
    HTTPResponse() : status_code(0) {}
};

// Callback function to write response data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, HTTPResponse* response) {
    size_t totalSize = size * nmemb;
    response->body.append((char*)contents, totalSize);
    return totalSize;
}

// HTTP Client class
class HTTPClient {
private:
    CURL* curl;
    
public:
    HTTPClient() {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();
    }
    
    ~HTTPClient() {
        if(curl) curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    
    HTTPResponse get(const string& url) {
        HTTPResponse response;
        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            
            CURLcode res = curl_easy_perform(curl);
            if(res == CURLE_OK) {
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);
            }
        }
        return response;
    }
    
    HTTPResponse post(const string& url, const string& data) {
        HTTPResponse response;
        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            
            struct curl_slist* headers = nullptr;
            headers = curl_slist_append(headers, "Content-Type: application/json");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            
            CURLcode res = curl_easy_perform(curl);
            if(res == CURLE_OK) {
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);
            }
            curl_slist_free_all(headers);
        }
        return response;
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
    
    Product(int id, const string& name, double price, int stock, 
           const string& category = "", const string& description = "")
        : id(id), name(name), price(price), stock(stock), 
          category(category), description(description) {}
    
    // Create from JSON
    static Product fromJson(const json& j) {
        return Product(
            j["id"],
            j["name"],
            j["price"],
            j["stock"],
            j.value("category", ""),
            j.value("description", "")
        );
    }
    
    // Convert to JSON
    json toJson() const {
        return json{
            {"id", id},
            {"name", name},
            {"price", price},
            {"stock", stock},
            {"category", category},
            {"description", description}
        };
    }
    
    void display() const {
        cout << "ID: " << id << "\n"
             << "Name: " << name << "\n"
             << "Price: $" << fixed << setprecision(2) << price/1000.0 << "k\n"
             << "Stock: " << stock << "\n"
             << "Category: " << category << "\n"
             << "Description: " << description << "\n"
             << "------------------------\n";
    }
};

// API Client for iShopefy
class iShopefyAPI {
private:
    HTTPClient client;
    string base_url;
    
public:
    iShopefyAPI(const string& url = "http://localhost:8000/api") 
        : base_url(url) {}
    
    // Get all products
    vector<Product> getProducts() {
        vector<Product> products;
        auto response = client.get(base_url + "/products.php");
        
        if(response.status_code == 200) {
            try {
                auto j = json::parse(response.body);
                if(j["status"] == "success") {
                    for(const auto& item : j["data"]) {
                        products.push_back(Product::fromJson(item));
                    }
                }
            } catch(const exception& e) {
                cerr << "JSON Parse Error: " << e.what() << endl;
            }
        } else {
            cerr << "HTTP Error: " << response.status_code << endl;
            cerr << "Response: " << response.body << endl;
        }
        
        return products;
    }
    
    // Get single product
    Product getProduct(int id) {
        auto response = client.get(base_url + "/products.php?id=" + to_string(id));
        
        if(response.status_code == 200) {
            try {
                auto j = json::parse(response.body);
                if(j["status"] == "success") {
                    return Product::fromJson(j["data"]);
                }
            } catch(const exception& e) {
                cerr << "JSON Parse Error: " << e.what() << endl;
            }
        }
        
        // Return empty product on error
        return Product(-1, "Not Found", 0, 0);
    }
    
    // Add new product
    bool addProduct(const Product& product) {
        json data = {
            {"name", product.name},
            {"price", product.price},
            {"stock", product.stock},
            {"category", product.category},
            {"description", product.description}
        };
        
        auto response = client.post(base_url + "/products.php", data.dump());
        return response.status_code == 200;
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
            cout << "1. View All Products\n";
            cout << "2. View Single Product\n";
            cout << "3. Add New Product\n";
            cout << "4. Exit\n";
            cout << "Choose option: ";
            cin >> choice;
            
            switch(choice) {
                case 1: viewAllProducts(); break;
                case 2: viewSingleProduct(); break;
                case 3: addNewProduct(); break;
                case 4: cout << "Goodbye!\n"; break;
                default: cout << "Invalid choice!\n";
            }
            cout << "\n";
        } while(choice != 4);
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
            cout << "Product added successfully!\n";
        } else {
            cout << "Failed to add product!\n";
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