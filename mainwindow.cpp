/******************************************************************
 * ENSC-151 GROUP 30 – Cafeteria Ordering System
 *
 * mainwindow.cpp
 *
 * This file implements the MainWindow class for a Qt-based
 * cafeteria ordering system. It supports:
 *   - Customer view: browse menu, add items to cart, checkout
 *   - Coupon and tax calculation
 *   - Manager view: add/remove items, edit prices, save menu
 *   - Secret numeric code to access manager view
 *
 ******************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QDateTime>   // ADDED: for timestamp in receipt (adapted from receipt.cpp)
#include <cmath>       // ADDED: for round2() using round (adapted from receipt.cpp)
using namespace std;

/******************************************************************
 * round2 --
 *   Helper function that rounds a double to 2 decimal places.
 *
 *   ADAPTED FROM Elliots's receipt.cpp.
 *   Original idea: format money values cleanly on a printed receipt.
 *   Same logic is used in our Qt-based receipt display.
 *
 * Parameters:
 *   num - double value to round
 *
 * Returns:
 *   double rounded to 2 decimal places
 ******************************************************************/
static double round2(double num)
{
    return round(num * 100.0) / 100.0;
}

/******************************************************************
 * MainWindow::MainWindow --
 *   Constructor. Sets up the UI, applies the style sheet, loads
 *   menu/coupon data, and initializes the starting customer view.
 *
 * Parameters:
 *   parent - pointer to parent widget (usually nullptr)
 *
 * Modifies:
 *   - UI widgets: icon sizes, style, combo box contents
 *   - Internal data structures: menuItems, coupons
 *
 * Returns: nothing
 ******************************************************************/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // Create all widgets from the .ui file
    ui->setupUi(this);

    // Make menu item icons larger in the customer list
    ui->itemsListWidget->setIconSize(QSize(64, 64));   // Large icons for customer menu
    ui->itemsListWidget->setSpacing(4);                // Small gap between rows

    // Slightly smaller icons for manager item list
    ui->managerItemsListWidget->setIconSize(QSize(32, 32));

    // Set window title shown in the title bar
    setWindowTitle("Cafeteria Ordering System");

    // Load window icon from the Qt resource file
    QIcon icon(":/images/images/logo.png");
    if (!icon.isNull()) {
        setWindowIcon(icon);
    }

    // Application-wide style sheet for a dark brown café theme
    QString styleSheet = R"(
    QMainWindow {
        background-color: #1a1410;
    }
    QWidget {
        background-color: #1a1410;
        color: #d4a574;
        font-family: 'Segoe UI', Arial;
        font-size: 11pt;
    }
    QGroupBox {
        background-color: #331e0e;
        border: 2px solid #4a3426;
        border-radius: 8px;
        margin-top: 12px;
        padding-top: 15px;
        font-weight: bold;
        color: #d4a574;
    }
    QGroupBox::title {
        subcontrol-origin: margin;
        subcontrol-position: top center;
        padding: 5px 10px;
        background-color: #3d2a1a;
        border-radius: 4px;
        color: #f4d4a4;
    }
    QLabel {
        background-color: transparent;
        color: #d4a574;
    }

    /* ===== QComboBox (category drop-down) ===== */
    QComboBox {
        background-color: #331e0e;
        color: #d4a574;
        border: 2px solid #4a3426;
        border-radius: 5px;
        padding: 5px;
        min-height: 25px;
    }
    QComboBox:hover {
        border: 2px solid #8b6f47;
    }
    QComboBox::drop-down {
        subcontrol-origin: padding;
        subcontrol-position: center right;
        width: 18px;
        background: transparent;
        border: none;
        margin: 0px;
        padding: 0px;
    }
    QComboBox::down-arrow {
        image: url(:/images/images/dropdown.png);
        width: 12px;
        height: 12px;
        margin-right: 4px;
    }
    QComboBox QAbstractItemView {
        background-color: #331e0e;
        color: #d4a574;
        selection-background-color: #4a3426;
        border: 2px solid #4a3426;
    }

    QListWidget {
        background-color: #331e0e;
        color: #d4a574;
        border: 2px solid #4a3426;
        border-radius: 5px;
        padding: 5px;
        font-size: 14pt;   /* Larger text for menu items */
    }
    QListWidget::item {
        padding: 8px;
        border-bottom: 1px solid #3d2a1a;
    }
    QListWidget::item:selected {
        background-color: #4a3426;
        color: #f4d4a4;
    }
    QListWidget::item:hover {
        background-color: #3d2a1a;
    }

    QTextEdit {
        background-color: #331e0e;
        color: #d4a574;
        border: 2px solid #4a3426;
        border-radius: 5px;
        padding: 10px;
        font-family: 'Courier New', monospace;
    }

    /* ===== QSpinBox (quantity) ===== */
    QSpinBox {
        background-color: #331e0e;
        color: #d4a574;
        border: 2px solid #4a3426;
        border-radius: 5px;
        padding-left: 5px;
        padding-right: 22px;      /* space for buttons */
        min-width: 60px;
    }
    QSpinBox::up-button {
        subcontrol-origin: border;
        subcontrol-position: top right;
        width: 18px;
        background: transparent;
        border: none;
        margin: 0px;
        padding: 0px;
    }
    QSpinBox::down-button {
        subcontrol-origin: border;
        subcontrol-position: bottom right;
        width: 18px;
        background: transparent;
        border: none;
        margin: 0px;
        padding: 0px;
    }
    QSpinBox::up-arrow {
        image: url(:/images/images/spinboxup.png);
        width: 10px;
        height: 10px;
    }
    QSpinBox::down-arrow {
        image: url(:/images/images/spinboxdown.png);
        width: 10px;
        height: 10px;
    }

    QPushButton {
        background-color: #4a3426;
        color: #f4d4a4;
        border: 2px solid #6b4d35;
        border-radius: 6px;
        padding: 10px;
        font-weight: bold;
        font-size: 11pt;
    }
    QPushButton:hover {
        background-color: #5d4230;
        border: 2px solid #8b6f47;
    }
    QPushButton:pressed {
        background-color: #3d2a1a;
    }
    QPushButton#addToCartButton {
        background-color: #3d5a2e;
        border: 2px solid #5a8040;
    }
    QPushButton#addToCartButton:hover {
        background-color: #4a6b39;
        border: 2px solid #6fa050;
    }
    QPushButton#checkoutButton {
        background-color: #2e4a5a;
        border: 2px solid #4070a0;
    }
    QPushButton#checkoutButton:hover {
        background-color: #39566b;
        border: 2px solid #5090c0;
    }
    QPushButton#clearCartButton {
        background-color: #5a2e2e;
        border: 2px solid #804040;
    }
    QPushButton#clearCartButton:hover {
        background-color: #6b3939;
        border: 2px solid #a05050;
    }
    QPushButton#addItemButton {
        background-color: #3d5a2e;
        border: 2px solid #5a8040;
    }
    QPushButton#addItemButton:hover {
        background-color: #4a6b39;
    }
    QPushButton#editPriceButton {
        background-color: #5a4a2e;
        border: 2px solid #807040;
    }
    QPushButton#editPriceButton:hover {
        background-color: #6b5939;
    }
    QPushButton#removeItemButton {
        background-color: #5a2e2e;
        border: 2px solid #804040;
    }
    QPushButton#removeItemButton:hover {
        background-color: #6b3939;
    }
    QPushButton#saveChangesButton {
        background-color: #2e4a5a;
        border: 2px solid #4070a0;
    }
    QPushButton#saveChangesButton:hover {
        background-color: #39566b;
    }
    QPushButton#managerBackButton {
        background-color: #3d3d3d;
        border: 2px solid #5a5a5a;
    }
    QPushButton#managerBackButton:hover {
        background-color: #4d4d4d;
    }
    QStatusBar {
        background-color: #2d1f14;
        color: #d4a574;
    }
)";

    // Apply the style sheet to the whole main window
    setStyleSheet(styleSheet);

    // Install event filter so we can detect secret numeric key sequence
    qApp->installEventFilter(this);

    // Load menu items and coupons from text files (or create defaults)
    loadMenuItems();
    loadCoupons();

    // Setup categories for the customer combo box
    ui->categoryComboBox->addItem("Main Dishes");
    ui->categoryComboBox->addItem("Side Items");
    ui->categoryComboBox->addItem("Beverages");
    ui->categoryComboBox->addItem("Desserts");

    // Initialize displays in customer view
    updateItemsList();
    updateCartDisplay();

    // Start program in customer view (not manager)
    switchToCustomerView();

    // Show a friendly status bar message
    statusBar()->showMessage("Welcome to Cafeteria Ordering System");
}

/******************************************************************
 * MainWindow::~MainWindow --
 *   Destructor. Cleans up UI pointer.
 *
 * Parameters: none
 *
 * Modifies: ui's target object (UI is destroyed and its memory freed)
 *
 * Returns: nothing
 ******************************************************************/
MainWindow::~MainWindow()
{
    delete ui;
}

// ========== FILE HANDLING ==========

/******************************************************************
 * MainWindow::loadMenuItems --
 *   Load menu items from the menu file. If the file does not exist,
 *   create a default menu with hard-coded items, then save it.
 *
 *   ADAPTED FROM Sai's "Food Menu.cpp":
 *     - Original was a console menu with these same items and prices.
 *     - Here, we store them in a QVector<FoodItem> with category and
 *       imagePath so they can be used in the Qt GUI.
 *
 * Parameters: none
 * Modifies:
 *   - menuItems: cleared and then filled with items
 *   - MENU_FILE: created/overwritten when default items are written
 *
 * Returns: nothing
 ******************************************************************/
void MainWindow::loadMenuItems()
{
    menuItems.clear();
    QFile file(MENU_FILE);

    // If file doesn't exist, create default menu items in memory
    if (!file.exists()) {
        FoodItem item;

        // Main Dishes
        item.name = "Cheese Burger"; item.price = 10.99; item.category = "Main Dishes"; item.imagePath = ":/images/images/Cheeseburger.png";
        menuItems.append(item);
        item.name = "Club Sandwich"; item.price = 10.99; item.category = "Main Dishes"; item.imagePath = ":/images/images/clubsandwitch.png";
        menuItems.append(item);
        item.name = "Macaroni and Cheese"; item.price = 8.99; item.category = "Main Dishes"; item.imagePath = ":/images/images/MacaroniandCheese.png";
        menuItems.append(item);
        item.name = "Chicken Strips"; item.price = 10.99; item.category = "Main Dishes"; item.imagePath = ":/images/images/ChickenStrips.png";
        menuItems.append(item);
        item.name = "Caesar Salad"; item.price = 8.99; item.category = "Main Dishes"; item.imagePath = ":/images/images/CaesarSalad.png";
        menuItems.append(item);
        item.name = "Spaghetti Bolognese"; item.price = 14.99; item.category = "Main Dishes"; item.imagePath = ":/images/images/SpaghettiBolognese.png";
        menuItems.append(item);
        item.name = "Chicken Wrap"; item.price = 10.99; item.category = "Main Dishes"; item.imagePath = ":/images/images/ChickenWrap.png";
        menuItems.append(item);
        item.name = "Breakfast Sandwich"; item.price = 10.99; item.category = "Main Dishes"; item.imagePath = ":/images/images/BreakfastSandwich.png";
        menuItems.append(item);

        // Side Items
        item.name = "Fries"; item.price = 3.99; item.category = "Side Items"; item.imagePath = ":/images/images/Fries.png";
        menuItems.append(item);
        item.name = "Mashed Potatoes"; item.price = 3.99; item.category = "Side Items"; item.imagePath = ":/images/images/MashedPotatoes.png";
        menuItems.append(item);
        item.name = "Roasted Vegetables"; item.price = 3.99; item.category = "Side Items"; item.imagePath = ":/images/images/RoastedVegetables.png";
        menuItems.append(item);
        item.name = "Hashbrowns"; item.price = 3.99; item.category = "Side Items"; item.imagePath = ":/images/images/Hashbrowns.png";
        menuItems.append(item);
        item.name = "Tater Tots"; item.price = 3.99; item.category = "Side Items"; item.imagePath = ":/images/images/TaterTots.png";
        menuItems.append(item);
        item.name = "Onion Rings"; item.price = 3.99; item.category = "Side Items"; item.imagePath = ":/images/images/OnionRings.png";
        menuItems.append(item);

        // Beverages
        item.name = "Soda"; item.price = 2.99; item.category = "Beverages"; item.imagePath = ":/images/images/Soda.png";
        menuItems.append(item);
        item.name = "Iced Tea"; item.price = 2.99; item.category = "Beverages"; item.imagePath = ":/images/images/IcedTea.png";
        menuItems.append(item);
        item.name = "Tea"; item.price = 2.99; item.category = "Beverages"; item.imagePath = ":/images/images/Tea.png";
        menuItems.append(item);
        item.name = "Coffee"; item.price = 4.99; item.category = "Beverages"; item.imagePath = ":/images/images/Coffee.png";
        menuItems.append(item);
        item.name = "Iced Coffee"; item.price = 4.99; item.category = "Beverages"; item.imagePath = ":/images/images/IcedCoffee.png";
        menuItems.append(item);
        item.name = "Milkshake"; item.price = 4.99; item.category = "Beverages"; item.imagePath = ":/images/images/Milkshake.png";
        menuItems.append(item);

        // Desserts
        item.name = "Chocolate Chip Cookie"; item.price = 4.99; item.category = "Desserts"; item.imagePath = ":/images/images/ChocolateChipCookie.png";
        menuItems.append(item);
        item.name = "Cheese Cake"; item.price = 7.99; item.category = "Desserts"; item.imagePath = ":/images/images/CheeseCake.png";
        menuItems.append(item);
        item.name = "Carrot Cake"; item.price = 7.99; item.category = "Desserts"; item.imagePath = ":/images/images/CarrotCake.png";
        menuItems.append(item);
        item.name = "Brownies"; item.price = 4.99; item.category = "Desserts"; item.imagePath = ":/images/images/Brownies.png";
        menuItems.append(item);
        item.name = "Apple Pie"; item.price = 7.99; item.category = "Desserts"; item.imagePath = ":/images/images/ApplePie.png";
        menuItems.append(item);
        item.name = "Banana Split"; item.price = 7.99; item.category = "Desserts"; item.imagePath = ":/images/images/BananaSplit.png";
        menuItems.append(item);
        item.name = "Tiramisu"; item.price = 7.99; item.category = "Desserts"; item.imagePath = ":/images/images/Tiramisu.png";
        menuItems.append(item);

        // Write default items to the menu file for next run
        saveMenuItems();
        return;
    }

    // If the file exists, load items line by line
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split(',');
            if (parts.size() >= 3) {
                FoodItem item;
                item.name = parts[0];
                item.price = parts[1].toDouble();
                item.category = parts[2];
                item.imagePath = (parts.size() == 4) ? parts[3] : "";
                menuItems.append(item);
            }
        }
        file.close();
    }
}

/******************************************************************
 * MainWindow::loadCoupons --
 *   Load coupon codes and discount percentages from file. If the
 *   file doesn't exist, default coupons are created and saved.
 *
 * Parameters: none
 * Modifies:
 *   - coupons: cleared and then filled with entries
 *   - COUPON_FILE: created/overwritten when default coupons written
 *
 * Returns: nothing
 ******************************************************************/
void MainWindow::loadCoupons()
{
    coupons.clear();
    QFile file(COUPON_FILE);

    // If file doesn't exist, create default coupon set
    if (!file.exists()) {
        coupons["10OFF"]   = 0.10;  // 10% off
        coupons["20OFF"]   = 0.20;  // 20% off
        coupons["SAVE15"]  = 0.15;  // 15% off
        coupons["STUDENT"] = 0.25;  // 25% off

        // Save default coupons to file
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            for (auto it = coupons.begin(); it != coupons.end(); ++it) {
                out << it.key() << "," << it.value() << "\n";
            }
            file.close();
        }
        return;
    }

    // Load coupons from file: CODE,discount
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split(',');
            if (parts.size() == 2) {
                coupons[parts[0]] = parts[1].toDouble();
            }
        }
        file.close();
    }
}

/******************************************************************
 * MainWindow::saveMenuItems --
 *   Save all current menu items to the menu file, one per line.
 *
 * Format:
 *   name,price,category,imagePath
 *
 * Parameters: none
 * Modifies:
 *   - MENU_FILE: overwritten with current menu contents
 *
 * Returns: nothing
 ******************************************************************/
void MainWindow::saveMenuItems()
{
    QFile file(MENU_FILE);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const FoodItem &item : menuItems) {
            out << item.name << "," << item.price << "," << item.category << "," << item.imagePath << "\n";
        }
        file.close();
    }
}

// ========== KEYBOARD EVENT HANDLING ==========

/******************************************************************
 * MainWindow::eventFilter --
 *   Global event filter used to detect a secret numeric pattern
 *   while in the customer view. When the pattern is typed, the
 *   manager password dialog is shown.
 *
 * Parameters:
 *   obj   - object receiving the event (ignored here)
 *   event - pointer to the event object
 *
 * Modifies:
 *   - keystrokeBuffer: appends numeric keys
 *   - UI: may switch to manager view on correct password
 *
 * Returns:
 *   true  - if the secret code event was handled
 *   false - otherwise (event passed to base class)
 ******************************************************************/
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);

    // Capture keyboard events in customer view only
    if (event->type() == QEvent::KeyPress && ui->stackedWidget->currentIndex() == 0) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        QString keyText = keyEvent->text();

        // Only process number keys (secret code is numeric)
        if (!keyText.isEmpty() && keyText[0].isDigit()) {
            keystrokeBuffer += keyText;

            // Keep only last 4 characters (length of "6677")
            if (keystrokeBuffer.length() > 4) {
                keystrokeBuffer = keystrokeBuffer.right(4);
            }

            // Show typed keys briefly in status bar (debug / hint)
            statusBar()->showMessage(QString("Keys: %1 (type '67' for manager access)").arg(keystrokeBuffer), 3000);

            // Check if secret code was entered
            if (keystrokeBuffer == SECRET_CODE) {
                keystrokeBuffer.clear();
                statusBar()->clearMessage();

                // Prompt for manager password
                bool ok;
                QString password = QInputDialog::getText(this, "Manager Access",
                                                         "Enter manager password:",
                                                         QLineEdit::Password,
                                                         "", &ok);

                if (ok && password == MANAGER_PASSWORD) {
                    // Correct password: go to manager mode
                    switchToManagerView();
                    updateManagerItemsList();
                    QMessageBox::information(this, "Manager Mode", "Welcome to Manager Mode!");
                } else if (ok) {
                    // User pressed OK but password was wrong
                    QMessageBox::warning(this, "Access Denied", "Incorrect password!");
                    statusBar()->showMessage("Welcome to Cafeteria Ordering System");
                } else {
                    // User cancelled dialog
                    statusBar()->showMessage("Welcome to Cafeteria Ordering System");
                }

                return true; // Event handled, do not pass further
            }
        }
    }

    // Pass event to default handler for all other cases
    return QMainWindow::eventFilter(obj, event);
}

/******************************************************************
 * MainWindow::keyPressEvent --
 *   Override of the base class key press handler. Currently just
 *   calls the base implementation; the real secret-code logic is
 *   in eventFilter().
 *
 * Parameters:
 *   event - key event
 *
 * Returns: nothing
 ******************************************************************/
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    QMainWindow::keyPressEvent(event);
}

// ========== VIEW SWITCHING ==========

/******************************************************************
 * MainWindow::switchToCustomerView --
 *   Switch the stacked widget to show the customer interface.
 *
 * Parameters: none
 * Modifies:
 *   - stackedWidget current index
 *   - window title
 *
 * Returns: nothing
 ******************************************************************/
void MainWindow::switchToCustomerView()
{
    ui->stackedWidget->setCurrentIndex(0);
    setWindowTitle("Cafeteria Ordering System");
}

/******************************************************************
 * MainWindow::switchToManagerView --
 *   Switch the stacked widget to show the manager interface.
 *
 * Parameters: none
 * Modifies:
 *   - stackedWidget current index
 *   - window title
 *
 * Returns: nothing
 ******************************************************************/
void MainWindow::switchToManagerView()
{
    ui->stackedWidget->setCurrentIndex(1);
    setWindowTitle("Cafeteria Ordering System - MANAGER MODE");
}

// ========== CUSTOMER MENU FUNCTIONS ==========

/******************************************************************
 * MainWindow::updateItemsList --
 *   Update the customer list of items based on the currently
 *   selected category in the combo box.
 *
 * Parameters: none
 * Modifies:
 *   - itemsListWidget: cleared and repopulated
 *
 * Returns: nothing
 ******************************************************************/
void MainWindow::updateItemsList()
{
    ui->itemsListWidget->clear();
    QString selectedCategory = ui->categoryComboBox->currentText();

    // Filter items by category and add them with icons
    for (const FoodItem &item : menuItems) {
        if (item.category == selectedCategory) {
            QString displayText = QString("%1 - $%2")
            .arg(item.name)
                .arg(item.price, 0, 'f', 2);
            QListWidgetItem *listItem = new QListWidgetItem(displayText);

            // Add icon if image path exists
            if (!item.imagePath.isEmpty()) {
                QIcon icon(item.imagePath);
                if (!icon.isNull()) {
                    listItem->setIcon(icon);
                }
            }

            // Set item height for better image + text spacing
            listItem->setSizeHint(QSize(0, 60));

            ui->itemsListWidget->addItem(listItem);
        }
    }
}

/******************************************************************
 * MainWindow::updateCartDisplay --
 *   Rebuild the cart text area based on the current cart contents.
 *
 * Parameters: none
 * Modifies:
 *   - cartTextEdit: shows a formatted summary of the cart and
 *                   current subtotal
 *
 * Returns: nothing
 ******************************************************************/
void MainWindow::updateCartDisplay()
{
    ui->cartTextEdit->clear();
    double subtotal = 0.0;

    QString cartText = "========== SHOPPING CART ==========\n\n";

    if (cart.isEmpty()) {
        cartText += "Your cart is empty.\n";
    } else {
        // List each item with quantity, price, and line total
        for (const OrderItem &item : cart) {
            double itemTotal = item.price * item.quantity;
            subtotal += itemTotal;
            cartText += QString("%1 x %2\n  @ $%3 each = $%4\n\n")
                            .arg(item.quantity)
                            .arg(item.name)
                            .arg(item.price, 0, 'f', 2)
                            .arg(itemTotal, 0, 'f', 2);
        }
        cartText += "-----------------------------------\n";
        cartText += QString("Subtotal: $%1\n").arg(subtotal, 0, 'f', 2);
    }

    ui->cartTextEdit->setPlainText(cartText);
}

/******************************************************************
 * MainWindow::on_categoryComboBox_currentIndexChanged --
 *   Slot called when the user selects a different category.
 *
 * Parameters:
 *   index - index of the selected category (unused)
 *
 * Modifies:
 *   - itemsListWidget: updated with new category items
 *
 * Returns: nothing
 ******************************************************************/
void MainWindow::on_categoryComboBox_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    updateItemsList();
}

/******************************************************************
 * MainWindow::on_addToCartButton_clicked --
 *   Slot called when the user presses "Add to Cart". It adds the
 *   selected menu item with the chosen quantity to the cart.
 *
 * Parameters: none
 * Modifies:
 *   - cart: item added or quantity increased
 *   - cartTextEdit: updated display
 *
 * Returns: nothing
 ******************************************************************/
void MainWindow::on_addToCartButton_clicked()
{
    // Get selected item from the menu list
    QListWidgetItem *selectedItem = ui->itemsListWidget->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, "No Selection", "Please select an item to add.");
        return;
    }

    // Get quantity from spin box
    int quantity = ui->quantitySpinBox->value();
    if (quantity <= 0) {
        QMessageBox::warning(this, "Invalid Quantity", "Please enter a quantity greater than 0.");
        return;
    }

    // Parse item name from display text "Name - $Price"
    QString displayText = selectedItem->text();
    QString itemName = displayText.split(" - ")[0];

    // Find the corresponding FoodItem in menuItems
    for (const FoodItem &item : menuItems) {
        if (item.name == itemName) {
            // Check if item already exists in cart
            bool found = false;
            for (OrderItem &orderItem : cart) {
                if (orderItem.name == itemName) {
                    orderItem.quantity += quantity;
                    found = true;
                    break;
                }
            }

            // If not in cart, add a brand new OrderItem
            if (!found) {
                OrderItem newItem;
                newItem.name = item.name;
                newItem.price = item.price;
                newItem.quantity = quantity;
                cart.append(newItem);
            }

            updateCartDisplay();
            QMessageBox::information(this, "Added to Cart",
                                     QString("Added %1 x %2 to cart!").arg(quantity).arg(itemName));
            ui->quantitySpinBox->setValue(1); // Reset quantity to 1
            return;
        }
    }
}

/******************************************************************
 * MainWindow::on_clearCartButton_clicked --
 *   Slot called when the user presses "Clear Cart". It asks for
 *   confirmation and then removes all items from the cart.
 *
 * Parameters: none
 * Modifies:
 *   - cart: cleared on confirmation
 *   - cartTextEdit: updated display
 *
 * Returns: nothing
 ******************************************************************/
void MainWindow::on_clearCartButton_clicked()
{
    if (cart.isEmpty()) {
        QMessageBox::information(this, "Cart Empty", "Your cart is already empty.");
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Clear Cart", "Are you sure you want to clear your cart?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        cart.clear();
        updateCartDisplay();
        QMessageBox::information(this, "Cart Cleared", "Your cart has been cleared.");
    }
}

/******************************************************************
 * MainWindow::on_checkoutButton_clicked --
 *   Slot called when the user presses "Checkout". It calculates
 *   subtotal, applies coupon (if valid), adds tax, then shows a
 *   formatted receipt.
 *
 * Parameters: none
 * Modifies:
 *   - cart: cleared after successful checkout
 *
 * Returns: nothing
 ******************************************************************/
void MainWindow::on_checkoutButton_clicked()
{
    if (cart.isEmpty()) {
        QMessageBox::warning(this, "Empty Cart", "Your cart is empty. Please add items before checkout.");
        return;
    }

    // Calculate subtotal
    double subtotal = 0.0;
    for (const OrderItem &item : cart) {
        subtotal += item.price * item.quantity;
    }

    // Ask user for optional coupon code
    bool ok;
    QString couponCode = QInputDialog::getText(this, "Coupon Code",
                                               "Enter coupon code (or leave blank):",
                                               QLineEdit::Normal,
                                               "", &ok);

    double discountPercent = 0.0;
    if (ok && !couponCode.isEmpty()) {
        couponCode = couponCode.toUpper();
        if (coupons.contains(couponCode)) {
            discountPercent = coupons[couponCode];
        } else {
            QMessageBox::warning(this, "Invalid Coupon", "Coupon code not recognized. Proceeding without discount.");
            couponCode.clear();
        }
    }

    // Calculate discount and apply it
    double discountAmount = subtotal * discountPercent;
    double afterDiscount = subtotal - discountAmount;

    // Add tax based on discounted amount
    double taxAmount = afterDiscount * TAX_RATE;

    // Final total
    double total = afterDiscount + taxAmount;

    // Show receipt dialog
    // NOTE: showReceipt() is now adapted from receipt.cpp and uses round2()
    // for consistent money formatting and adds date/time at the bottom.
    showReceipt(subtotal, discountAmount, taxAmount, total, couponCode);

    // Clear cart for next customer
    cart.clear();
    updateCartDisplay();
}

/******************************************************************
 * MainWindow::showReceipt --
 *   Build a text receipt showing each item, the subtotal, discount,
 *   tax, and total, then display it in a QMessageBox.
 *
 *   ADAPTED FROM Elliot's receipt.cpp:
 *     - Kept the idea of listing items and showing subtotal, tax,
 *       and total with clean 2-decimal formatting.
 *     - REMOVED his separate 7% PST. This program only applies
 *       a single 5% tax based on BC tax on food (TAX_RATE).
 *     - Uses round2() for all money values (same logic as their
 *       console version).
 *     - Adds date and time at the bottom using QDateTime instead
 *       of <ctime> since we are using Qt.
 *
 * Parameters:
 *   subtotal   - sum of item costs before discount
 *   discount   - discount amount in dollars
 *   tax        - tax amount in dollars (already 5% of discounted)
 *   total      - final amount to pay
 *   couponCode - text of coupon applied (may be empty)
 *
 * Modifies:
 *   - Shows a dialog box with the receipt text
 *
 * Returns: nothing
 ******************************************************************/
void MainWindow::showReceipt(double subtotal, double discount, double tax, double total, QString couponCode)
{
    // Round all monetary values to 2 decimal places (adapted from receipt.cpp)
    subtotal = round2(subtotal);
    discount = round2(discount);
    tax      = round2(tax);
    total    = round2(total);

    QString receipt;
    receipt += "========================================\n";
    receipt += "           CAFETERIA RECEIPT\n";
    receipt += "========================================\n\n";

    // Header row similar in spirit to teammate's receipt (Item / Price)
    receipt += QString("%1%2%3\n")
                   .arg("Qty",  -5)
                   .arg("Item", -20)
                   .arg("Price", 10);
    receipt += "----------------------------------------\n";

    // List all items in the cart (one row per OrderItem)
    for (const OrderItem &item : cart) {
        double itemTotal = item.price * item.quantity;

        // Each row: quantity, name (trimmed to 20 chars), line total
        receipt += QString("%1%2%3\n")
                       .arg(item.quantity, -5)
                       .arg(item.name.left(20), -20)
                       .arg(itemTotal, 10, 'f', 2);
    }

    receipt += "----------------------------------------\n";

    // Show subtotal
    receipt += QString("%1%2\n")
                   .arg("Subtotal:", -25)
                   .arg(subtotal, 10, 'f', 2);

    // Show discount if any
    if (discount > 0.0) {
        QString label = QString("Discount (%1):").arg(couponCode);
        receipt += QString("%1-%2\n")
                       .arg(label, -25)
                       .arg(discount, 9, 'f', 2);
    }

    // NOTE: Only 5% tax is used (TAX_RATE).
    // Teammate's receipt.cpp had both GST (5%) and PST (7%).
    receipt += QString("%1%2\n")
                   .arg("Tax (5%):", -25)
                   .arg(tax, 10, 'f', 2);

    receipt += "----------------------------------------\n";
    receipt += QString("%1%2\n")
                   .arg("TOTAL:", -25)
                   .arg(total, 10, 'f', 2);
    receipt += "========================================\n\n";

    // Add date and time at the bottom (Qt version of ctime in receipt.cpp)
    QDateTime now = QDateTime::currentDateTime();
    receipt += "Date and Time: " + now.toString("yyyy-MM-dd hh:mm:ss") + "\n";
    receipt += "========================================\n";

    QMessageBox receiptBox;
    receiptBox.setWindowTitle("Order Receipt");
    receiptBox.setText(receipt);
    receiptBox.setIcon(QMessageBox::Information);
    receiptBox.exec();
}

// ========== MANAGER MENU FUNCTIONS ==========

/******************************************************************
 * MainWindow::updateManagerItemsList --
 *   Repopulate the manager list widget with all menu items.
 *
 * Parameters: none
 * Modifies:
 *   - managerItemsListWidget: cleared and filled with "[category] name - $price"
 *
 * Returns: nothing
 ******************************************************************/
void MainWindow::updateManagerItemsList()
{
    ui->managerItemsListWidget->clear();

    for (const FoodItem &item : menuItems) {
        QString displayText = QString("[%1] %2 - $%3")
        .arg(item.category)
            .arg(item.name)
            .arg(item.price, 0, 'f', 2);
        ui->managerItemsListWidget->addItem(displayText);
    }
}

/******************************************************************
 * MainWindow::on_managerBackButton_clicked --
 *   Slot for the "Back" button in manager view. Switches back to
 *   customer view and refreshes the menu list in case changes were
 *   made.
 *
 * Parameters: none
 * Modifies:
 *   - current stacked widget page
 *   - itemsListWidget: refreshed
 *
 * Returns: nothing
 ******************************************************************/
void MainWindow::on_managerBackButton_clicked()
{
    switchToCustomerView();
    updateItemsList(); // Refresh in case prices or items changed
}

/******************************************************************
 * MainWindow::on_addItemButton_clicked --
 *   Slot for "Add Item" in manager view. Prompts for name, price,
 *   and category, then adds a new food item with no image.
 *
 * Parameters: none
 * Modifies:
 *   - menuItems: appended with new item
 *
 * Returns: nothing
 ******************************************************************/
void MainWindow::on_addItemButton_clicked()
{
    // Get item name from manager
    bool ok;
    QString name = QInputDialog::getText(this, "Add Item", "Enter item name:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;

    // Get price for the new item
    double price = QInputDialog::getDouble(this, "Add Item", "Enter price:", 0.00, 0.00, 10000.00, 2, &ok);
    if (!ok) return;

    // Let manager choose which category the new item belongs to
    QStringList categories;
    categories << "Main Dishes" << "Side Items" << "Beverages" << "Desserts";
    QString category = QInputDialog::getItem(this, "Add Item", "Select category:", categories, 0, false, &ok);
    if (!ok) return;

    // Add item to internal menu; no image is assigned by default
    FoodItem newItem;
    newItem.name = name;
    newItem.price = price;
    newItem.category = category;
    newItem.imagePath = "";  // No image for manually added items
    menuItems.append(newItem);

    updateManagerItemsList();
    QMessageBox::information(this, "Success", "Item added successfully!");
}

/******************************************************************
 * MainWindow::on_removeItemButton_clicked --
 *   Slot for "Remove Item" in manager view. Deletes the selected
 *   item from menuItems after confirmation.
 *
 * Parameters: none
 * Modifies:
 *   - menuItems: one element removed
 *
 * Returns: nothing
 ******************************************************************/
void MainWindow::on_removeItemButton_clicked()
{
    QListWidgetItem *selectedItem = ui->managerItemsListWidget->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, "No Selection", "Please select an item to remove.");
        return;
    }

    // Parse item name from text "[Category] Name - $Price"
    QString displayText = selectedItem->text();
    QString itemName = displayText.split("] ")[1].split(" - ")[0];

    // Confirm deletion with the manager
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Deletion",
                                  QString("Are you sure you want to remove '%1'?").arg(itemName),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // Search menuItems for that name and remove it
        for (int i = 0; i < menuItems.size(); ++i) {
            if (menuItems[i].name == itemName) {
                menuItems.removeAt(i);
                break;
            }
        }

        updateManagerItemsList();
        QMessageBox::information(this, "Success", "Item removed successfully!");
    }
}

/******************************************************************
 * MainWindow::on_editPriceButton_clicked --
 *   Slot for "Edit Price" in manager view. Prompts for a new price
 *   for the selected item and updates menuItems.
 *
 * Parameters: none
 * Modifies:
 *   - menuItems: price of one item changed
 *
 * Returns: nothing
 ******************************************************************/
void MainWindow::on_editPriceButton_clicked()
{
    QListWidgetItem *selectedItem = ui->managerItemsListWidget->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, "No Selection", "Please select an item to edit.");
        return;
    }

    // Parse item name from "[Category] Name - $price"
    QString displayText = selectedItem->text();
    QString itemName = displayText.split("] ")[1].split(" - ")[0];

    // Find the item in menuItems and ask for a new price
    for (FoodItem &item : menuItems) {
        if (item.name == itemName) {
            bool ok;
            double newPrice = QInputDialog::getDouble(this, "Edit Price",
                                                      QString("Enter new price for %1:").arg(itemName),
                                                      item.price, 0.00, 10000.00, 2, &ok);

            if (ok) {
                item.price = newPrice;
                updateManagerItemsList();
                QMessageBox::information(this, "Success", "Price updated successfully!");
            }
            return;
        }
    }
}

/******************************************************************
 * MainWindow::on_saveChangesButton_clicked --
 *   Slot for "Save Changes" in manager view. Writes the current
 *   menuItems vector to the menu file.
 *
 * Parameters: none
 * Modifies:
 *   - MENU_FILE: overwritten
 *
 * Returns: nothing
 ******************************************************************/
void MainWindow::on_saveChangesButton_clicked()
{
    saveMenuItems();
    QMessageBox::information(this, "Success", "All changes saved to file!");
}

