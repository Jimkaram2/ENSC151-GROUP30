#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QString>
#include <QVector>
#include <QKeyEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// Structure to hold food item information
struct FoodItem {
    QString name;
    double price;
    QString category;
    QString imagePath;  // Path to item image
};

// Structure to hold order items
struct OrderItem {
    QString name;
    double price;
    int quantity;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    // Override keyPressEvent to detect "6767" pattern
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    // Customer menu slots
    void on_categoryComboBox_currentIndexChanged(int index);
    void on_addToCartButton_clicked();
    void on_checkoutButton_clicked();
    void on_clearCartButton_clicked();

    // Manager menu slots
    void on_managerBackButton_clicked();
    void on_addItemButton_clicked();
    void on_removeItemButton_clicked();
    void on_editPriceButton_clicked();
    void on_saveChangesButton_clicked();

private:
    Ui::MainWindow *ui;

    // Data structures
    QVector<FoodItem> menuItems;           // All food items
    QVector<OrderItem> cart;                // Customer's cart
    QMap<QString, double> coupons;          // Coupon codes and discounts

    // Secret access tracking
    QString keystrokeBuffer;                // Buffer to track "6677" pattern
    const QString SECRET_CODE = "6677";     // Secret keystroke pattern
    const QString MANAGER_PASSWORD = "admin123";  // Manager password

    // File paths
    const QString MENU_FILE = "menu_items.txt";
    const QString COUPON_FILE = "coupons.txt";

    // Tax rate
    const double TAX_RATE = 0.05;  // 5% tax

    // Helper functions
    void loadMenuItems();           // Load menu from file
    void loadCoupons();             // Load coupons from file
    void saveMenuItems();           // Save menu to file
    void updateItemsList();         // Update items display for selected category
    void updateCartDisplay();       // Update shopping cart display
    void updateManagerItemsList();  // Update manager's item list
    void switchToCustomerView();    // Show customer interface
    void switchToManagerView();     // Show manager interface
    void showReceipt(double subtotal, double discount, double tax, double total, QString couponCode);
};

#endif // MAINWINDOW_H
