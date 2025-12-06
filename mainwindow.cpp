#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Set window title
    setWindowTitle("Cafeteria Ordering System");

    // For Windows: "C:/Users/YourUsername/Downloads/logo.png"
    QIcon icon(":/images/images/logo.png");
    if (!icon.isNull()) {
        setWindowIcon(icon);
    }

    // Apply dark brown/black theme
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
            background-color: #2d1f14;
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
        QComboBox {
            background-color: #2d1f14;
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
            border: none;
            width: 30px;
        }
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 5px solid #d4a574;
            margin-right: 10px;
        }
        QComboBox QAbstractItemView {
            background-color: #2d1f14;
            color: #d4a574;
            selection-background-color: #4a3426;
            border: 2px solid #4a3426;
        }
        QListWidget {
            background-color: #2d1f14;
            color: #d4a574;
            border: 2px solid #4a3426;
            border-radius: 5px;
            padding: 5px;
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
            background-color: #2d1f14;
            color: #d4a574;
            border: 2px solid #4a3426;
            border-radius: 5px;
            padding: 10px;
            font-family: 'Courier New', monospace;
        }
        QSpinBox {
            background-color: #2d1f14;
            color: #d4a574;
            border: 2px solid #4a3426;
            border-radius: 5px;
            padding: 5px;
            min-width: 60px;
        }
        QSpinBox::up-button, QSpinBox::down-button {
            background-color: #3d2a1a;
            border: none;
            width: 20px;
        }
        QSpinBox::up-button:hover, QSpinBox::down-button:hover {
            background-color: #4a3426;
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

    setStyleSheet(styleSheet);

    // Install event filter to catch number keys globally
    qApp->installEventFilter(this);

    // Load data from files
    loadMenuItems();
    loadCoupons();

    // Setup category combo box
    ui->categoryComboBox->addItem("Main Dishes");
    ui->categoryComboBox->addItem("Side Items");
    ui->categoryComboBox->addItem("Beverages");
    ui->categoryComboBox->addItem("Desserts");

    // Initialize displays
    updateItemsList();
    updateCartDisplay();

    // Start in customer view
    switchToCustomerView();

    // Show hint in status bar
    statusBar()->showMessage("Welcome to Cafeteria Ordering System");
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ========== FILE HANDLING ==========

void MainWindow::loadMenuItems()
{
    menuItems.clear();
    QFile file(MENU_FILE);

    // If file doesn't exist, create default menu
    if (!file.exists()) {
        FoodItem item;

        // Main Dishes
        item.name = "Hamburger"; item.price = 8.99; item.category = "Main Dishes"; item.imagePath = ":/images/images/hamburger.png";
        menuItems.append(item);
        item.name = "Cheeseburger"; item.price = 9.99; item.category = "Main Dishes"; item.imagePath = ":/images/images/cheeseburger.png";
        menuItems.append(item);
        item.name = "Chicken Sandwich"; item.price = 10.49; item.category = "Main Dishes"; item.imagePath = ":/images/images/chicken.png";
        menuItems.append(item);
        item.name = "Pizza Slice"; item.price = 4.99; item.category = "Main Dishes"; item.imagePath = ":/images/images/pizza.png";
        menuItems.append(item);
        item.name = "Pasta Bowl"; item.price = 11.99; item.category = "Main Dishes"; item.imagePath = ":/images/images/pasta.png";
        menuItems.append(item);

        // Side Items
        item.name = "French Fries"; item.price = 3.49; item.category = "Side Items"; item.imagePath = ":/images/images/fries.png";
        menuItems.append(item);
        item.name = "Onion Rings"; item.price = 3.99; item.category = "Side Items"; item.imagePath = ":/images/images/onionrings.png";
        menuItems.append(item);
        item.name = "Side Salad"; item.price = 4.49; item.category = "Side Items"; item.imagePath = ":/images/images//salad.png";
        menuItems.append(item);
        item.name = "Coleslaw"; item.price = 2.99; item.category = "Side Items"; item.imagePath = ":/images/images/coleslaw.png";
        menuItems.append(item);

        // Beverages
        item.name = "Soft Drink"; item.price = 2.49; item.category = "Beverages"; item.imagePath = ":/images/images/soda.png";
        menuItems.append(item);
        item.name = "Bottled Water"; item.price = 1.99; item.category = "Beverages"; item.imagePath = ":/images/images/water.png";
        menuItems.append(item);
        item.name = "Coffee"; item.price = 2.99; item.category = "Beverages"; item.imagePath = ":/images/images/coffee.png";
        menuItems.append(item);
        item.name = "Juice"; item.price = 3.49; item.category = "Beverages"; item.imagePath = ":/images/images/juice.png";
        menuItems.append(item);

        // Desserts
        item.name = "Chocolate Cake"; item.price = 4.99; item.category = "Desserts"; item.imagePath = ":/images/images/cake.png";
        menuItems.append(item);
        item.name = "Ice Cream"; item.price = 3.99; item.category = "Desserts"; item.imagePath = ":/images/images/icecream.png";
        menuItems.append(item);
        item.name = "Apple Pie"; item.price = 4.49; item.category = "Desserts"; item.imagePath = ":/images/images/pie.png";
        menuItems.append(item);
        item.name = "Cookies"; item.price = 2.99; item.category = "Desserts"; item.imagePath = ":/images/images/cookies.png";
        menuItems.append(item);

        saveMenuItems();
        return;
    }

    // Load from file
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

void MainWindow::loadCoupons()
{
    coupons.clear();
    QFile file(COUPON_FILE);

    // If file doesn't exist, create default coupons
    if (!file.exists()) {
        coupons["10OFF"] = 0.10;  // 10% off
        coupons["20OFF"] = 0.20;  // 20% off
        coupons["SAVE15"] = 0.15; // 15% off
        coupons["STUDENT"] = 0.25; // 25% off

        // Save to file
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            for (auto it = coupons.begin(); it != coupons.end(); ++it) {
                out << it.key() << "," << it.value() << "\n";
            }
            file.close();
        }
        return;
    }

    // Load from file
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

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    // Capture keyboard events in customer view only
    if (event->type() == QEvent::KeyPress && ui->stackedWidget->currentIndex() == 0) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        QString keyText = keyEvent->text();

        // Only process number keys
        if (!keyText.isEmpty() && keyText[0].isDigit()) {
            keystrokeBuffer += keyText;

            // Keep only last 4 characters (length of "6677")
            if (keystrokeBuffer.length() > 4) {
                keystrokeBuffer = keystrokeBuffer.right(4);
            }

            // DEBUG: Show what's been typed in status bar
            statusBar()->showMessage(QString("Keys: %1 (type '67' for manager access)").arg(keystrokeBuffer), 3000);

            // Check if secret code was entered
            if (keystrokeBuffer == SECRET_CODE) {
                keystrokeBuffer.clear();
                statusBar()->clearMessage();

                // Show password dialog
                bool ok;
                QString password = QInputDialog::getText(this, "Manager Access",
                                                         "Enter manager password:",
                                                         QLineEdit::Password,
                                                         "", &ok);

                if (ok && password == MANAGER_PASSWORD) {
                    switchToManagerView();
                    updateManagerItemsList();
                    QMessageBox::information(this, "Manager Mode", "Welcome to Manager Mode!");
                } else if (ok) {
                    QMessageBox::warning(this, "Access Denied", "Incorrect password!");
                    statusBar()->showMessage("Welcome to Cafeteria Ordering System");
                } else {
                    // User cancelled
                    statusBar()->showMessage("Welcome to Cafeteria Ordering System");
                }

                return true; // Event handled
            }
        }
    }

    // Pass event to base class
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    QMainWindow::keyPressEvent(event);
}

// ========== VIEW SWITCHING ==========

void MainWindow::switchToCustomerView()
{
    ui->stackedWidget->setCurrentIndex(0);
    setWindowTitle("Cafeteria Ordering System");
}

void MainWindow::switchToManagerView()
{
    ui->stackedWidget->setCurrentIndex(1);
    setWindowTitle("Cafeteria Ordering System - MANAGER MODE");
}

// ========== CUSTOMER MENU FUNCTIONS ==========

void MainWindow::updateItemsList()
{
    ui->itemsListWidget->clear();
    QString selectedCategory = ui->categoryComboBox->currentText();

    // Filter items by category and add with icons
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

            // Set item height for better image display
            listItem->setSizeHint(QSize(0, 60));

            ui->itemsListWidget->addItem(listItem);
        }
    }
}

void MainWindow::updateCartDisplay()
{
    ui->cartTextEdit->clear();
    double subtotal = 0.0;

    QString cartText = "========== SHOPPING CART ==========\n\n";

    if (cart.isEmpty()) {
        cartText += "Your cart is empty.\n";
    } else {
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

void MainWindow::on_categoryComboBox_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    updateItemsList();
}

void MainWindow::on_addToCartButton_clicked()
{
    // Get selected item
    QListWidgetItem *selectedItem = ui->itemsListWidget->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, "No Selection", "Please select an item to add.");
        return;
    }

    // Get quantity
    int quantity = ui->quantitySpinBox->value();
    if (quantity <= 0) {
        QMessageBox::warning(this, "Invalid Quantity", "Please enter a quantity greater than 0.");
        return;
    }

    // Parse item name from display text
    QString displayText = selectedItem->text();
    QString itemName = displayText.split(" - ")[0];

    // Find the item in menuItems
    for (const FoodItem &item : menuItems) {
        if (item.name == itemName) {
            // Check if item already in cart
            bool found = false;
            for (OrderItem &orderItem : cart) {
                if (orderItem.name == itemName) {
                    orderItem.quantity += quantity;
                    found = true;
                    break;
                }
            }

            // If not in cart, add new entry
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
            ui->quantitySpinBox->setValue(1);
            return;
        }
    }
}

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

    // Ask for coupon code
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

    // Calculate discount
    double discountAmount = subtotal * discountPercent;
    double afterDiscount = subtotal - discountAmount;

    // Calculate tax
    double taxAmount = afterDiscount * TAX_RATE;

    // Calculate total
    double total = afterDiscount + taxAmount;

    // Show receipt
    showReceipt(subtotal, discountAmount, taxAmount, total, couponCode);

    // Clear cart after checkout
    cart.clear();
    updateCartDisplay();
}

void MainWindow::showReceipt(double subtotal, double discount, double tax, double total, QString couponCode)
{
    QString receipt = "========================================\n";
    receipt += "         CAFETERIA RECEIPT\n";
    receipt += "========================================\n\n";

    // List all items
    for (const OrderItem &item : cart) {
        double itemTotal = item.price * item.quantity;
        receipt += QString("%1 x %2\n")
                       .arg(item.quantity, 3)
                       .arg(item.name);
        receipt += QString("    @ $%1 each          $%2\n\n")
                       .arg(item.price, 0, 'f', 2)
                       .arg(itemTotal, 0, 'f', 2);
    }

    receipt += "----------------------------------------\n";
    receipt += QString("Subtotal:                     $%1\n").arg(subtotal, 0, 'f', 2);

    if (discount > 0.0) {
        receipt += QString("Discount (%1):             -$%2\n")
        .arg(couponCode)
            .arg(discount, 0, 'f', 2);
    }

    receipt += QString("Tax (5%%):                     $%1\n").arg(tax, 0, 'f', 2);
    receipt += "----------------------------------------\n";
    receipt += QString("TOTAL:                        $%1\n").arg(total, 0, 'f', 2);
    receipt += "========================================\n";
    receipt += "\n     Thank you for your order!\n";
    receipt += "========================================\n";

    QMessageBox receiptBox;
    receiptBox.setWindowTitle("Order Receipt");
    receiptBox.setText(receipt);
    receiptBox.setIcon(QMessageBox::Information);
    receiptBox.exec();
}

// ========== MANAGER MENU FUNCTIONS ==========

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

void MainWindow::on_managerBackButton_clicked()
{
    switchToCustomerView();
    updateItemsList(); // Refresh in case prices changed
}

void MainWindow::on_addItemButton_clicked()
{
    // Get item name
    bool ok;
    QString name = QInputDialog::getText(this, "Add Item", "Enter item name:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;

    // Get price
    double price = QInputDialog::getDouble(this, "Add Item", "Enter price:", 0.00, 0.00, 10000.00, 2, &ok);
    if (!ok) return;

    // Get category
    QStringList categories;
    categories << "Main Dishes" << "Side Items" << "Beverages" << "Desserts";
    QString category = QInputDialog::getItem(this, "Add Item", "Select category:", categories, 0, false, &ok);
    if (!ok) return;

    // Add item
    FoodItem newItem;
    newItem.name = name;
    newItem.price = price;
    newItem.category = category;
    newItem.imagePath = "";  // No image for manually added items
    menuItems.append(newItem);

    updateManagerItemsList();
    QMessageBox::information(this, "Success", "Item added successfully!");
}

void MainWindow::on_removeItemButton_clicked()
{
    QListWidgetItem *selectedItem = ui->managerItemsListWidget->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, "No Selection", "Please select an item to remove.");
        return;
    }

    // Parse item name
    QString displayText = selectedItem->text();
    QString itemName = displayText.split("] ")[1].split(" - ")[0];

    // Confirm deletion
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Deletion",
                                  QString("Are you sure you want to remove '%1'?").arg(itemName),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // Remove from menuItems
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

void MainWindow::on_editPriceButton_clicked()
{
    QListWidgetItem *selectedItem = ui->managerItemsListWidget->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, "No Selection", "Please select an item to edit.");
        return;
    }

    // Parse item name
    QString displayText = selectedItem->text();
    QString itemName = displayText.split("] ")[1].split(" - ")[0];

    // Find item and get new price
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

void MainWindow::on_saveChangesButton_clicked()
{
    saveMenuItems();
    QMessageBox::information(this, "Success", "All changes saved to file!");
}
