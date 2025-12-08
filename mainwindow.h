/******************************************************************
 * mainwindow.h
 *
 * This header file declares the MainWindow class, which controls
 * the main GUI for the cafeteria ordering system. It also defines
 * simple data structures for menu items and order items.
 *
 ******************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QString>
#include <QVector>
#include <QKeyEvent>

QT_BEGIN_NAMESPACE
// Forward declaration of the auto-generated UI class from Qt Designer
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/******************************************************************
 * FoodItem
 *
 * Simple struct used to store information about a single item
 * on the cafeteria menu.
 *
 * Members:
 *   name      - name of the item (e.g., "Cheese Burger")
 *   price     - price of the item in dollars
 *   category  - menu category (e.g., "Main Dishes", "Beverages")
 *   imagePath - resource path for the item's icon image
 ******************************************************************/
struct FoodItem {
    QString name;
    double price;
    QString category;
    QString imagePath;
};

/******************************************************************
 * OrderItem
 *
 * Struct used to store items that the customer has added to
 * their cart during the ordering process.
 *
 * Members:
 *   name      - name of the item
 *   price     - price of one unit of the item
 *   quantity  - how many of this item are in the cart
 ******************************************************************/
struct OrderItem {
    QString name;
    double price;
    int quantity;
};

/******************************************************************
 * MainWindow
 *
 * The MainWindow class is the main GUI window for the program.
 * It displays the customer menu, handles the shopping cart, and
 * provides a hidden manager-only interface for editing menu data.
 ******************************************************************/
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**************************************************************
     * Constructor / Destructor
     *
     * MainWindow(QWidget *parent = 0)
     *   - Creates and initializes the main window.
     *
     * ~MainWindow()
     *   - Cleans up any dynamically allocated resources.
     **************************************************************/
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    /**************************************************************
     * keyPressEvent --
     *   Overrides the default key press handler. We keep this
     *   function so that future custom key handling can be added.
     *
     * eventFilter --
     *   Global event filter used to watch for the secret key
     *   pattern ("6677") while the customer view is active.
     **************************************************************/
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    /**************************************************************
     * CUSTOMER VIEW SLOTS
     *
     * These slots are connected to widgets on the customer screen.
     * They are called automatically when the user interacts with
     * the combo box or buttons.
     **************************************************************/

    /**********************************************************
     * on_categoryComboBox_currentIndexChanged(int index)
     *
     * Triggered when:
     *   - The user selects a different category from the
     *     category drop-down (QComboBox) in the customer view.
     *
     * Purpose:
     *   - Calls updateItemsList() so that the list of menu items
     *     on the left updates to show only items in the selected
     *     category (e.g., "Main Dishes", "Side Items").
     **********************************************************/
    void on_categoryComboBox_currentIndexChanged(int index);

    /**********************************************************
     * on_addToCartButton_clicked()
     *
     * Triggered when:
     *   - The user clicks the "Add to Cart" button.
     *
     * Purpose:
     *   - Reads which menu item is currently selected and the
     *     quantity from the spin box.
     *   - Adds that item (and quantity) to the cart, or, if the
     *     item is already in the cart, increases its quantity.
     *   - Calls updateCartDisplay() to refresh the cart window.
     **********************************************************/
    void on_addToCartButton_clicked();

    /**********************************************************
     * on_checkoutButton_clicked()
     *
     * Triggered when:
     *   - The user clicks the "Checkout" button.
     *
     * Purpose:
     *   - Calculates subtotal, applies any valid coupon, adds tax,
     *     and computes the final total.
     *   - Calls showReceipt(...) to display a formatted receipt.
     *   - Clears the cart after checkout is complete.
     **********************************************************/
    void on_checkoutButton_clicked();

    /**********************************************************
     * on_clearCartButton_clicked()
     *
     * Triggered when:
     *   - The user clicks the "Clear Cart" button.
     *
     * Purpose:
     *   - Asks the user for confirmation.
     *   - If confirmed, removes all items from the cart and
     *     calls updateCartDisplay() to show an empty cart.
     **********************************************************/
    void on_clearCartButton_clicked();

    /**************************************************************
     * MANAGER VIEW SLOTS
     *
     * These slots are connected to the buttons in the manager
     * interface. They allow the cafeteria manager to modify the
     * menu (add, edit, delete items) and save changes.
     **************************************************************/

    /**********************************************************
     * on_managerBackButton_clicked()
     *
     * Triggered when:
     *   - The manager clicks the "Back" button in manager mode.
     *
     * Purpose:
     *   - Switches the screen back to the customer view by calling
     *     switchToCustomerView().
     *   - Also calls updateItemsList() so any edited prices or
     *     items appear correctly for customers.
     **********************************************************/
    void on_managerBackButton_clicked();

    /**********************************************************
     * on_addItemButton_clicked()
     *
     * Triggered when:
     *   - The manager clicks the "Add Item" button.
     *
     * Purpose:
     *   - Uses input dialogs to ask the manager for:
     *       1) new item's name
     *       2) price
     *       3) category (e.g., Main, Side, Beverage, Dessert)
     *   - Creates a new FoodItem and appends it to menuItems.
     *   - Calls updateManagerItemsList() so the new item appears
     *     immediately in the manager list.
     **********************************************************/
    void on_addItemButton_clicked();

    /**********************************************************
     * on_removeItemButton_clicked()
     *
     * Triggered when:
     *   - The manager selects an item in the manager list and
     *     clicks the "Remove Item" button.
     *
     * Purpose:
     *   - Finds the selected item by its name.
     *   - Asks the manager to confirm deletion.
     *   - If confirmed, removes that FoodItem from menuItems.
     *   - Calls updateManagerItemsList() to refresh the list.
     **********************************************************/
    void on_removeItemButton_clicked();

    /**********************************************************
     * on_editPriceButton_clicked()
     *
     * Triggered when:
     *   - The manager selects an item in the manager list and
     *     clicks the "Edit Price" button.
     *
     * Purpose:
     *   - Finds the matching FoodItem in menuItems.
     *   - Prompts the manager for a new price using an input
     *     dialog.
     *   - Updates the item's price and refreshes the manager list
     *     by calling updateManagerItemsList().
     **********************************************************/
    void on_editPriceButton_clicked();

    /**********************************************************
     * on_saveChangesButton_clicked()
     *
     * Triggered when:
     *   - The manager clicks the "Save Changes" button.
     *
     * Purpose:
     *   - Calls saveMenuItems() to write the current menuItems
     *     vector to the menu_items.txt file so that additions,
     *     removals, and price changes are saved between runs.
     **********************************************************/
    void on_saveChangesButton_clicked();

private:
    // Pointer to the auto-generated UI object (from Qt Designer)
    Ui::MainWindow *ui;

    /**************************************************************
     * Data structures for the application
     **************************************************************/
    QVector<FoodItem> menuItems;   // All food items available
    QVector<OrderItem> cart;       // Items currently in customer's cart
    QMap<QString, double> coupons; // Coupon codes mapped to discount % (0.10 = 10%)

    /**************************************************************
     * Manager access and security settings
     **************************************************************/
    QString keystrokeBuffer;                 // Stores recent key presses
    const QString SECRET_CODE = "6677";      // Hidden key pattern for manager mode
    const QString MANAGER_PASSWORD = "admin123";  // Simple manager password

    /**************************************************************
     * File paths used to store persistent data
     **************************************************************/
    const QString MENU_FILE   = "menu_items.txt";  // Menu items file
    const QString COUPON_FILE = "coupons.txt";     // Coupon codes file

    /**************************************************************
     * Tax rate (5% for British Columbia food tax)
     **************************************************************/
    const double TAX_RATE = 0.05;

    /**************************************************************
     * Helper functions (internal use only)
     *
     * loadMenuItems()        - reads menu data from MENU_FILE or
     *                          creates defaults if the file is missing.
     * loadCoupons()          - reads coupon codes and discount values.
     * saveMenuItems()        - writes the current menu to MENU_FILE.
     * updateItemsList()      - refreshes the list of items shown for
     *                          the selected category.
     * updateCartDisplay()    - refreshes the shopping cart text box.
     * updateManagerItemsList()- refreshes the list of items in the
     *                          manager view.
     * switchToCustomerView() - shows the customer-facing interface.
     * switchToManagerView()  - shows the manager-only interface.
     * showReceipt()       - builds and displays a text receipt after
     *                          checkout.
     **************************************************************/
    void loadMenuItems();
    void loadCoupons();
    void saveMenuItems();
    void updateItemsList();
    void updateCartDisplay();
    void updateManagerItemsList();
    void switchToCustomerView();
    void switchToManagerView();
    void showReceipt(double subtotal, double discount, double tax, double total, QString couponCode);
};

#endif // MAINWINDOW_H
