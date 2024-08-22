#include <iostream>
#include <pqxx/pqxx> // PostgreSQL C++ library
#include <string>
#include <vector>

// User Class
class User {
private:
    int userId;
    std::string userName;
    std::string address;
public:
    User(int id, std::string name, std::string addr) : userId(id), userName(name), address(addr) {}

    int getUserId() { return userId; }
    std::string getUserName() { return userName; }
    std::string getAddress() { return address; }
};

// Restaurant Class
class Restaurant {
private:
    int restaurantId;
    std::string restaurantName;
    std::string location;
public:
    Restaurant(int id, std::string name, std::string loc) : restaurantId(id), restaurantName(name), location(loc) {}

    int getRestaurantId() { return restaurantId; }
    std::string getRestaurantName() { return restaurantName; }
    std::string getLocation() { return location; }
};

// FoodItem Class
class FoodItem {
private:
    int itemId;
    std::string itemName;
    double price;
public:
    FoodItem(int id, std::string name, double price) : itemId(id), itemName(name), price(price) {}

    int getItemId() { return itemId; }
    std::string getItemName() { return itemName; }
    double getPrice() { return price; }
};

// Order Class
class Order {
private:
    int orderId;
    User user;
    Restaurant restaurant;
    std::vector<FoodItem> items;
public:
    Order(int id, User usr, Restaurant rest, std::vector<FoodItem> foodItems)
        : orderId(id), user(usr), restaurant(rest), items(foodItems) {}

    int getOrderId() { return orderId; }
    User getUser() { return user; }
    Restaurant getRestaurant() { return restaurant; }
    std::vector<FoodItem> getItems() { return items; }
};

// Database Class
class Database {
private:
    pqxx::connection *conn;
public:
    Database(const std::string &connectionStr) {
        conn = new pqxx::connection(connectionStr);
        if (conn->is_open()) {
            std::cout << "Opened database successfully: " << conn->dbname() << std::endl;
        } else {
            std::cerr << "Can't open database" << std::endl;
            exit(1);
        }
    }

    ~Database() {
        conn->disconnect();
        delete conn;
    }

    pqxx::connection* getConnection() {
        return conn;
    }
};

// Function to Add User to Database
void addUserToDatabase(Database &db, User &user) {
    pqxx::work W(*db.getConnection());
    std::string sql = "INSERT INTO users (ID, NAME, ADDRESS) VALUES (" +
                      std::to_string(user.getUserId()) + ", '" +
                      W.esc(user.getUserName()) + "', '" +
                      W.esc(user.getAddress()) + "');";

    W.exec(sql);
    W.commit();
    std::cout << "User added successfully" << std::endl;
}

// Function to Place an Order
void placeOrder(Database &db, Order &order) {
    pqxx::work W(*db.getConnection());
    std::string sql = "INSERT INTO orders (ID, USER_ID, RESTAURANT_ID) VALUES (" +
                      std::to_string(order.getOrderId()) + ", " +
                      std::to_string(order.getUser().getUserId()) + ", " +
                      std::to_string(order.getRestaurant().getRestaurantId()) + ");";

    W.exec(sql);
    for (auto &item : order.getItems()) {
        std::string itemSql = "INSERT INTO order_items (ORDER_ID, ITEM_ID) VALUES (" +
                              std::to_string(order.getOrderId()) + ", " +
                              std::to_string(item.getItemId()) + ");";
        W.exec(itemSql);
    }
    W.commit();
    std::cout << "Order placed successfully" << std::endl;
}

// Main Function
int main() {
    // PostgreSQL connection string
    std::string connectionStr = "dbname=fooddelivery user=sowmyathirupathi password=sowmyathirupathi hostaddr=127.0.0.1 port=5432";

    // Initialize the database connection
    Database db(connectionStr);

    // Create some users, restaurants, and food items
    User user1(1, "John Doe", "123 Street");
    Restaurant rest1(1, "Pizza Place", "456 Avenue");
    FoodItem item1(1, "Pepperoni Pizza", 9.99);
    FoodItem item2(2, "Coke", 1.99);

    // Add the user to the database
    addUserToDatabase(db, user1);

    //  an orderPlace
    std::vector<FoodItem> items = {item1, item2};
    Order order1(1, user1, rest1, items);
    placeOrder(db, order1);

    return 0;
}
