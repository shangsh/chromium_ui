#ifdef AURA_MAC

#import "controls/controls_mac.h"
#include <functional>
#include <string>

// ============================================================================
// ObjC Target classes - bridge native events to C++ std::function
// MUST be at global scope (ObjC++ requirement)
// ============================================================================

@interface AuraButtonTarget : NSObject
@property (nonatomic, assign) std::function<void()>* onClickFn;
- (void)onButtonClick:(id)sender;
@end

@interface AuraTextFieldTarget : NSObject
@property (nonatomic, assign) std::function<void()>* onChangeFn;
- (void)onTextChange:(id)sender;
@end

@interface AuraSliderTarget : NSObject
@property (nonatomic, assign) std::function<void(double)>* onChangeFn;
- (void)onSliderChange:(id)sender;
@end

@interface AuraCheckboxTarget : NSObject
@property (nonatomic, assign) std::function<void()>* onChangeFn;
- (void)onCheckboxChange:(id)sender;
@end

@interface AuraComboBoxTarget : NSObject
@property (nonatomic, assign) std::function<void()>* onChangeFn;
- (void)onComboBoxChange:(id)sender;
@end

@interface AuraListViewDataSource : NSObject <NSTableViewDataSource, NSTableViewDelegate>
@property (nonatomic, assign) std::function<void(int)>* onSelectFn;
@property (nonatomic, assign) std::function<void(int)>* onDoubleClickFn;
@property (nonatomic, assign) std::vector<std::vector<std::wstring>>* rowsPtr;
- (void)onSelect:(id)sender;
@end

@implementation AuraButtonTarget
@synthesize onClickFn;
- (void)onButtonClick:(id)sender {
    if (self->onClickFn && *self->onClickFn) (*self->onClickFn)();
}
@end

@implementation AuraTextFieldTarget
@synthesize onChangeFn;
- (void)onTextChange:(id)sender {
    if (self->onChangeFn && *self->onChangeFn) (*self->onChangeFn)();
}
@end

@implementation AuraSliderTarget
@synthesize onChangeFn;
- (void)onSliderChange:(id)sender {
    if (self->onChangeFn && *self->onChangeFn) {
        NSSlider* slider = (NSSlider*)sender;
        (*self->onChangeFn)([slider doubleValue]);
    }
}
@end

@implementation AuraCheckboxTarget
@synthesize onChangeFn;
- (void)onCheckboxChange:(id)sender {
    if (self->onChangeFn && *self->onChangeFn) (*self->onChangeFn)();
}
@end

@implementation AuraComboBoxTarget
@synthesize onChangeFn;
- (void)onComboBoxChange:(id)sender {
    if (self->onChangeFn && *self->onChangeFn) (*self->onChangeFn)();
}
@end

@implementation AuraListViewDataSource
@synthesize onSelectFn, onDoubleClickFn, rowsPtr;

- (void)onSelect:(id)sender {
    NSTableView* tv = (NSTableView*)sender;
    NSInteger row = [tv selectedRow];
    if (row >= 0 && self->onSelectFn && *self->onSelectFn) {
        (*self->onSelectFn)((int)row);
    }
}

- (void)tableViewSelectionDidChange:(NSNotification*)notification {
    NSTableView* tv = (NSTableView*)[notification object];
    NSInteger row = [tv selectedRow];
    if (row >= 0 && self->onSelectFn && *self->onSelectFn) {
        (*self->onSelectFn)((int)row);
    }
}

- (BOOL)tableView:(NSTableView*)tableView shouldSelectRow:(NSInteger)row {
    return YES;
}

- (id)tableView:(NSTableView*)tableView objectValueForTableColumn:(NSTableColumn*)col row:(NSInteger)row {
    if (!self->rowsPtr) return @"";
    size_t r = static_cast<size_t>(row);
    size_t c = 0;
    NSString* ident = [col identifier];
    if (ident) {
        c = [[col identifier] intValue];
    }
    if (r < self->rowsPtr->size() && c < (*self->rowsPtr)[r].size()) {
        std::wstring ws = (*self->rowsPtr)[r][c];
        std::string s8(ws.begin(), ws.end());
        return [NSString stringWithUTF8String:s8.c_str()];
    }
    return @"";
}

- (NSInteger)numberOfRowsInTableView:(NSTableView*)tableView {
    if (!self->rowsPtr) return 0;
    return static_cast<NSInteger>(self->rowsPtr->size());
}

@end

namespace aura {
namespace mac {

// ============================================================================
// ControlMac base
// ============================================================================

void ControlMac::SetVisible(bool visible) {
    if (nsview_) [nsview_ setHidden:!visible];
}

void ControlMac::SetEnabled(bool enabled) {
    // Subclasses handle this (NSControl has setEnabled)
}

// ============================================================================
// ButtonMac
// ============================================================================

ButtonMac::ButtonMac() {}
ButtonMac::~ButtonMac() {
    if (button_) { [button_ release]; button_ = nil; }
    if (target_) { [target_ release]; target_ = nil; }
}

void ButtonMac::Init(int x, int y, int width, int height) {
    if (button_) return;
    button_ = [[NSButton alloc] initWithFrame:NSMakeRect(x, y, width, height)];
    [button_ setButtonType:NSButtonTypeMomentaryPushIn];
    [button_ setBezelStyle:NSBezelStyleRounded];
    [button_ setTitle:@""];
    if (onClick_) {
        target_ = [[AuraButtonTarget alloc] init];
        [(AuraButtonTarget*)target_ setOnClickFn:&onClick_];
        [button_ setTarget:target_];
        [button_ setAction:@selector(onButtonClick:)];
    }
    nsview_ = button_;
}

void ButtonMac::SetText(const std::wstring& text) {
    text_ = text;
    if (button_) {
        NSString* s = [NSString stringWithUTF8String:std::string(text.begin(), text.end()).c_str()];
        [button_ setTitle:s];
    }
}

void ButtonMac::SetOnClick(std::function<void()> fn) {
    onClick_ = fn;
    if (button_ && fn) {
        if (!target_) target_ = [[AuraButtonTarget alloc] init];
        [(AuraButtonTarget*)target_ setOnClickFn:&onClick_];
        [button_ setTarget:target_];
        [button_ setAction:@selector(onButtonClick:)];
    }
}

void ButtonMac::SetBounds(int x, int y, int width, int height) {
    if (button_) [button_ setFrame:NSMakeRect(x, y, width, height)];
}

// ============================================================================
// LabelMac
// ============================================================================

LabelMac::LabelMac() : label_(nil) {}
LabelMac::LabelMac(const std::wstring& text) : label_(nil), text_(text) {}
LabelMac::~LabelMac() {
    if (label_) { [label_ release]; label_ = nil; }
}

void LabelMac::Init(int x, int y, int width, int height) {
    if (label_) return;
    label_ = [[NSTextField alloc] initWithFrame:NSMakeRect(x, y, width, height)];
    [label_ setStringValue:@""];
    [label_ setBezeled:NO];
    [label_ setDrawsBackground:NO];
    [label_ setEditable:NO];
    [label_ setSelectable:NO];
    nsview_ = label_;
}

void LabelMac::Init(int x, int y, int width, int height, const std::wstring& text) {
    Init(x, y, width, height);
    SetText(text);
}

void LabelMac::SetText(const std::wstring& text) {
    text_ = text;
    if (label_) {
        NSString* s = [NSString stringWithUTF8String:std::string(text.begin(), text.end()).c_str()];
        [label_ setStringValue:s];
    }
}

void LabelMac::SetFont(NSFont* font) {
    if (label_ && font) [label_ setFont:font];
}

void LabelMac::SetTextColor(NSColor* color) {
    if (label_ && color) [label_ setTextColor:color];
}

void LabelMac::SetAlignment(NSTextAlignment align) {
    if (label_) [label_ setAlignment:align];
}

void LabelMac::SetBounds(int x, int y, int width, int height) {
    if (label_) [label_ setFrame:NSMakeRect(x, y, width, height)];
}

// ============================================================================
// TextFieldMac
// ============================================================================

TextFieldMac::TextFieldMac() : password_(false), read_only_(false), target_(nil) {
    textfield_ = nil;
}
TextFieldMac::~TextFieldMac() {
    if (textfield_) { [textfield_ release]; textfield_ = nil; }
    if (target_) { [target_ release]; target_ = nil; }
}

void TextFieldMac::Init(int x, int y, int width, int height) {
    if (textfield_) return;
    textfield_ = [[NSTextField alloc] initWithFrame:NSMakeRect(x, y, width, height)];
    [textfield_ setStringValue:@""];
    [textfield_ setBezeled:YES];
    [textfield_ setBezelStyle:NSTextFieldSquareBezel];
    [textfield_ setEditable:YES];
    if (!placeholder_.empty()) {
        NSString* ph = [NSString stringWithUTF8String:std::string(placeholder_.begin(), placeholder_.end()).c_str()];
        [textfield_ setPlaceholderString:ph];
    }
    nsview_ = textfield_;
}

void TextFieldMac::SetText(const std::wstring& text) {
    if (textfield_) {
        NSString* s = [NSString stringWithUTF8String:std::string(text.begin(), text.end()).c_str()];
        [textfield_ setStringValue:s];
    }
}

std::wstring TextFieldMac::GetText() const {
    if (textfield_) {
        NSString* s = [textfield_ stringValue];
        if (s) {
            std::string s8 = [s UTF8String];
            return std::wstring(s8.begin(), s8.end());
        }
    }
    return L"";
}

void TextFieldMac::SetPlaceholder(const std::wstring& text) {
    placeholder_ = text;
    if (textfield_) {
        NSString* ph = [NSString stringWithUTF8String:std::string(text.begin(), text.end()).c_str()];
        [textfield_ setPlaceholderString:ph];
    }
}

void TextFieldMac::SetPasswordMode(bool password) {
    password_ = password;
    if (textfield_) {
        if (password) {
            NSRect frame = [textfield_ frame];
            NSSecureTextField* secure = [[NSSecureTextField alloc] initWithFrame:frame];
            [secure setFont:[textfield_ font]];
            if (!placeholder_.empty()) {
                NSString* ph = [NSString stringWithUTF8String:std::string(placeholder_.begin(), placeholder_.end()).c_str()];
                [secure setPlaceholderString:ph];
            }
            [textfield_ release];
            textfield_ = secure;
            nsview_ = secure;
        }
    }
}

void TextFieldMac::SetReadOnly(bool read_only) {
    read_only_ = read_only;
    if (textfield_) [textfield_ setEditable:!read_only];
}

void TextFieldMac::SetOnChange(std::function<void()> fn) {
    onChange_ = fn;
    if (textfield_ && fn) {
        if (!target_) target_ = [[AuraTextFieldTarget alloc] init];
        [(AuraTextFieldTarget*)target_ setOnChangeFn:&onChange_];
        [textfield_ setTarget:target_];
        [textfield_ setAction:@selector(onTextChange:)];
    }
}

void TextFieldMac::SetBounds(int x, int y, int width, int height) {
    if (textfield_) [textfield_ setFrame:NSMakeRect(x, y, width, height)];
}

// ============================================================================
// CheckboxMac
// ============================================================================

CheckboxMac::CheckboxMac() : checkbox_(nil), target_(nil) {}
CheckboxMac::CheckboxMac(const std::wstring& text) : checkbox_(nil), text_(text), target_(nil) {}
CheckboxMac::~CheckboxMac() {
    if (checkbox_) { [checkbox_ release]; checkbox_ = nil; }
    if (target_) { [target_ release]; target_ = nil; }
}

void CheckboxMac::Init(int x, int y, int width, int height) {
    if (checkbox_) return;
    checkbox_ = [[NSButton alloc] initWithFrame:NSMakeRect(x, y, width, height)];
    [checkbox_ setButtonType:NSButtonTypeSwitch];
    [checkbox_ setTitle:@""];
    nsview_ = checkbox_;
}

void CheckboxMac::Init(int x, int y, int width, int height, const std::wstring& text) {
    Init(x, y, width, height);
    SetText(text);
}

void CheckboxMac::SetText(const std::wstring& text) {
    text_ = text;
    if (checkbox_) {
        NSString* s = [NSString stringWithUTF8String:std::string(text.begin(), text.end()).c_str()];
        [checkbox_ setTitle:s];
    }
}

void CheckboxMac::SetChecked(bool checked) {
    if (checkbox_) [checkbox_ setState:checked ? NSControlStateValueOn : NSControlStateValueOff];
}

bool CheckboxMac::IsChecked() const {
    if (checkbox_) return [checkbox_ state] == NSControlStateValueOn;
    return false;
}

void CheckboxMac::SetOnChange(std::function<void()> fn) {
    onChange_ = fn;
    if (checkbox_ && fn) {
        if (!target_) target_ = [[AuraCheckboxTarget alloc] init];
        [(AuraCheckboxTarget*)target_ setOnChangeFn:&onChange_];
        [checkbox_ setTarget:target_];
        [checkbox_ setAction:@selector(onCheckboxChange:)];
    }
}

void CheckboxMac::SetBounds(int x, int y, int width, int height) {
    if (checkbox_) [checkbox_ setFrame:NSMakeRect(x, y, width, height)];
}

// ============================================================================
// SliderMac
// ============================================================================

SliderMac::SliderMac() : slider_(nil), min_(0.0), max_(100.0), target_(nil) {}
SliderMac::~SliderMac() {
    if (slider_) { [slider_ release]; slider_ = nil; }
    if (target_) { [target_ release]; target_ = nil; }
}

void SliderMac::Init(int x, int y, int width, int height) {
    if (slider_) return;
    slider_ = [[NSSlider alloc] initWithFrame:NSMakeRect(x, y, width, height)];
    [slider_ setMinValue:min_];
    [slider_ setMaxValue:max_];
    [slider_ setDoubleValue:(min_ + max_) * 0.5];
    [slider_ setNumberOfTickMarks:0];
    [slider_ setAllowsTickMarkValuesOnly:NO];
    if (onChange_) {
        target_ = [[AuraSliderTarget alloc] init];
        [(AuraSliderTarget*)target_ setOnChangeFn:&onChange_];
        [slider_ setTarget:target_];
        [slider_ setAction:@selector(onSliderChange:)];
    }
    nsview_ = slider_;
}

void SliderMac::SetValue(double value) {
    if (slider_) [slider_ setDoubleValue:value];
}

double SliderMac::GetValue() const {
    if (slider_) return [slider_ doubleValue];
    return min_;
}

void SliderMac::SetMinValue(double min) {
    min_ = min;
    if (slider_) [slider_ setMinValue:min];
}

void SliderMac::SetMaxValue(double max) {
    max_ = max;
    if (slider_) [slider_ setMaxValue:max];
}

void SliderMac::SetOnChange(std::function<void(double)> fn) {
    onChange_ = fn;
    if (slider_ && fn) {
        if (!target_) target_ = [[AuraSliderTarget alloc] init];
        [(AuraSliderTarget*)target_ setOnChangeFn:&onChange_];
        [slider_ setTarget:target_];
        [slider_ setAction:@selector(onSliderChange:)];
    }
}

void SliderMac::SetBounds(int x, int y, int width, int height) {
    if (slider_) [slider_ setFrame:NSMakeRect(x, y, width, height)];
}

// ============================================================================
// ScrollViewMac
// ============================================================================

ScrollViewMac::ScrollViewMac() : scroll_view_(nil), content_view_(nil) {}
ScrollViewMac::~ScrollViewMac() {
    if (scroll_view_) { [scroll_view_ release]; scroll_view_ = nil; }
}

void ScrollViewMac::Init(int x, int y, int width, int height) {
    if (scroll_view_) return;
    scroll_view_ = [[NSScrollView alloc] initWithFrame:NSMakeRect(x, y, width, height)];
    [scroll_view_ setHasVerticalScroller:YES];
    [scroll_view_ setHasHorizontalScroller:YES];
    [scroll_view_ setBorderType:NSBezelBorder];
    [scroll_view_ setAutohidesScrollers:NO];
    nsview_ = scroll_view_;
}

void ScrollViewMac::SetContentView(NSView* view) {
    content_view_ = view;
    if (scroll_view_) [scroll_view_ setDocumentView:view];
}

void ScrollViewMac::SetHasVerticalScrollBar(bool has) {
    if (scroll_view_) [scroll_view_ setHasVerticalScroller:has];
}

void ScrollViewMac::SetHasHorizontalScrollBar(bool has) {
    if (scroll_view_) [scroll_view_ setHasHorizontalScroller:has];
}

void ScrollViewMac::SetScrollPosition(int x, int y) {
    if (content_view_) [content_view_ scrollPoint:NSMakePoint(x, y)];
}

void ScrollViewMac::ScrollToBottom() {
    if (content_view_) {
        NSPoint maxPt = NSMakePoint(0, [content_view_ frame].size.height);
        [content_view_ scrollPoint:maxPt];
    }
}

void ScrollViewMac::SetBounds(int x, int y, int width, int height) {
    if (scroll_view_) [scroll_view_ setFrame:NSMakeRect(x, y, width, height)];
}

// ============================================================================
// ListViewMac - NSTableView wrapper
// ============================================================================

ListViewMac::ListViewMac() : table_(nil), scroll_view_(nil), datasource_(nil), delegate_(nil) {}
ListViewMac::~ListViewMac() {
    if (table_) { [table_ release]; table_ = nil; }
    if (scroll_view_) { [scroll_view_ release]; scroll_view_ = nil; }
    if (datasource_) { [datasource_ release]; datasource_ = nil; }
    if (delegate_) { [delegate_ release]; delegate_ = nil; }
}

void ListViewMac::Init(int x, int y, int width, int height) {
    if (table_) return;
    
    scroll_view_ = [[NSScrollView alloc] initWithFrame:NSMakeRect(x, y, width, height)];
    [scroll_view_ setHasVerticalScroller:YES];
    [scroll_view_ setHasHorizontalScroller:YES];
    [scroll_view_ setBorderType:NSBezelBorder];
    
    table_ = [[NSTableView alloc] init];
    [table_ setUsesAlternatingRowBackgroundColors:YES];
    [table_ setAllowsColumnReordering:NO];
    [table_ setAllowsMultipleSelection:NO];
    [table_ setGridStyleMask:NSTableViewSolidHorizontalGridLineMask];
    
    datasource_ = [[AuraListViewDataSource alloc] init];
    [(AuraListViewDataSource*)datasource_ setRowsPtr:&rows_];
    [table_ setDataSource:datasource_];
    [table_ setDelegate:datasource_];
    
    [scroll_view_ setDocumentView:table_];
    nsview_ = scroll_view_;
}

void ListViewMac::AddColumn(const std::wstring& title, int width) {
    if (!table_) return;
    NSString* nsTitle = [NSString stringWithUTF8String:std::string(title.begin(), title.end()).c_str()];
    NSTableColumn* col = [[NSTableColumn alloc] initWithIdentifier:[NSString stringWithFormat:@"%d", (int)[[table_ tableColumns] count]]];
    [col setTitle:nsTitle];
    [col setWidth:width];
    [col setResizingMask:NSTableColumnAutoresizingMask | NSTableColumnUserResizingMask];
    [table_ addTableColumn:col];
}

void ListViewMac::AddRow(const std::vector<std::wstring>& cells) {
    rows_.push_back(cells);
    if (table_) [table_ reloadData];
}

void ListViewMac::ClearRows() {
    rows_.clear();
    if (table_) [table_ reloadData];
}

int ListViewMac::GetSelectedRow() const {
    if (table_) return (int)[table_ selectedRow];
    return -1;
}

void ListViewMac::SelectRow(int index) {
    if (table_ && index >= 0 && index < (int)rows_.size()) {
        [table_ selectRowIndexes:[NSIndexSet indexSetWithIndex:index] byExtendingSelection:NO];
    }
}

void ListViewMac::RemoveSelectedRow() {
    int row = GetSelectedRow();
    if (row >= 0 && row < (int)rows_.size()) {
        rows_.erase(rows_.begin() + row);
        if (table_) [table_ reloadData];
    }
}

void ListViewMac::SetOnSelect(std::function<void(int)> fn) {
    onSelect_ = fn;
    if (datasource_ && fn) {
        [(AuraListViewDataSource*)datasource_ setOnSelectFn:&onSelect_];
    }
}

void ListViewMac::SetOnDoubleClick(std::function<void(int)> fn) {
    onDoubleClick_ = fn;
    if (datasource_ && fn) {
        [(AuraListViewDataSource*)datasource_ setOnDoubleClickFn:&onDoubleClick_];
    }
}

void ListViewMac::SetBounds(int x, int y, int width, int height) {
    if (scroll_view_) [scroll_view_ setFrame:NSMakeRect(x, y, width, height)];
}

// ============================================================================
// ComboBoxMac
// ============================================================================

ComboBoxMac::ComboBoxMac() : combobox_(nil), target_(nil) {}
ComboBoxMac::~ComboBoxMac() {
    if (combobox_) { [combobox_ release]; combobox_ = nil; }
    if (target_) { [target_ release]; target_ = nil; }
}

void ComboBoxMac::Init(int x, int y, int width, int height) {
    if (combobox_) return;
    combobox_ = [[NSComboBox alloc] initWithFrame:NSMakeRect(x, y, width, height)];
    [combobox_ setEditable:NO];
    [combobox_ setUsesDataSource:NO];
    if (onChange_) {
        target_ = [[AuraComboBoxTarget alloc] init];
        [(AuraComboBoxTarget*)target_ setOnChangeFn:&onChange_];
        [combobox_ setTarget:target_];
        [combobox_ setAction:@selector(onComboBoxChange:)];
    }
    nsview_ = combobox_;
}

void ComboBoxMac::AddItem(const std::wstring& item) {
    items_.push_back(item);
    if (combobox_) {
        NSString* s = [NSString stringWithUTF8String:std::string(item.begin(), item.end()).c_str()];
        [combobox_ addItemWithObjectValue:s];
    }
}

void ComboBoxMac::InsertItem(int index, const std::wstring& item) {
    if (index < 0 || index > (int)items_.size()) return;
    items_.insert(items_.begin() + index, item);
    if (combobox_) {
        NSString* s = [NSString stringWithUTF8String:std::string(item.begin(), item.end()).c_str()];
        [combobox_ insertItemWithObjectValue:s atIndex:index];
    }
}

void ComboBoxMac::RemoveItem(int index) {
    if (index < 0 || index >= (int)items_.size()) return;
    items_.erase(items_.begin() + index);
    if (combobox_) [combobox_ removeItemAtIndex:index];
}

void ComboBoxMac::ClearItems() {
    items_.clear();
    if (combobox_) [combobox_ removeAllItems];
}

void ComboBoxMac::SelectItem(int index) {
    if (combobox_ && index >= 0 && index < (int)items_.size()) {
        [combobox_ selectItemAtIndex:index];
    }
}

int ComboBoxMac::GetSelectedIndex() const {
    if (combobox_) return (int)[combobox_ indexOfSelectedItem];
    return -1;
}

std::wstring ComboBoxMac::GetSelectedItem() const {
    int idx = GetSelectedIndex();
    if (idx >= 0 && idx < (int)items_.size()) return items_[idx];
    return L"";
}

void ComboBoxMac::SetEditable(bool editable) {
    if (combobox_) [combobox_ setEditable:editable];
}

bool ComboBoxMac::IsEditable() const {
    if (combobox_) return [combobox_ isEditable];
    return false;
}

void ComboBoxMac::SetOnChange(std::function<void()> fn) {
    onChange_ = fn;
    if (combobox_ && fn) {
        if (!target_) target_ = [[AuraComboBoxTarget alloc] init];
        [(AuraComboBoxTarget*)target_ setOnChangeFn:&onChange_];
        [combobox_ setTarget:target_];
        [combobox_ setAction:@selector(onComboBoxChange:)];
    }
}

void ComboBoxMac::SetBounds(int x, int y, int width, int height) {
    if (combobox_) [combobox_ setFrame:NSMakeRect(x, y, width, height)];
}

// ============================================================================
// ProgressBarMac
// ============================================================================

ProgressBarMac::ProgressBarMac() : progress_(nil), value_(0.0), min_(0.0), max_(100.0), indeterminate_(false) {}
ProgressBarMac::~ProgressBarMac() {
    if (progress_) { [progress_ release]; progress_ = nil; }
}

void ProgressBarMac::Init(int x, int y, int width, int height) {
    if (progress_) return;
    progress_ = [[NSProgressIndicator alloc] initWithFrame:NSMakeRect(x, y, width, height)];
    [progress_ setMinValue:min_];
    [progress_ setMaxValue:max_];
    [progress_ setDoubleValue:value_];
    [progress_ setIndeterminate:indeterminate_];
    [progress_ setStyle:NSProgressIndicatorStyleBar];
    nsview_ = progress_;
}

void ProgressBarMac::SetValue(double value) {
    value_ = value;
    if (progress_) {
        [progress_ setIndeterminate:NO];
        [progress_ setDoubleValue:value];
    }
}

void ProgressBarMac::SetMinValue(double min) {
    min_ = min;
    if (progress_) [progress_ setMinValue:min];
}

void ProgressBarMac::SetMaxValue(double max) {
    max_ = max;
    if (progress_) [progress_ setMaxValue:max];
}

void ProgressBarMac::SetIndeterminate(bool indeterminate) {
    indeterminate_ = indeterminate;
    if (progress_) [progress_ setIndeterminate:indeterminate];
}

void ProgressBarMac::StartAnimation() {
    if (progress_) {
        [progress_ setIndeterminate:YES];
        [progress_ startAnimation:nil];
    }
}

void ProgressBarMac::StopAnimation() {
    if (progress_) [progress_ stopAnimation:nil];
}

void ProgressBarMac::SetBounds(int x, int y, int width, int height) {
    if (progress_) [progress_ setFrame:NSMakeRect(x, y, width, height)];
}

// ============================================================================
// SeparatorMac
// ============================================================================

SeparatorMac::SeparatorMac() : separator_(nil), horizontal_(true) {}
SeparatorMac::~SeparatorMac() {
    if (separator_) { [separator_ release]; separator_ = nil; }
}

void SeparatorMac::Init(int x, int y, int width, int height) {
    if (separator_) return;
    separator_ = [[NSBox alloc] initWithFrame:NSMakeRect(x, y, width, height)];
    [separator_ setBoxType:NSBoxSeparator];
    [separator_ setTransparent:YES];
    nsview_ = separator_;
}

void SeparatorMac::InitHorizontal(int x, int y, int width) {
    Init(x, y, width, 1);
    horizontal_ = true;
}

void SeparatorMac::InitVertical(int x, int y, int height) {
    if (separator_) { [separator_ release]; separator_ = nil; }
    separator_ = [[NSBox alloc] initWithFrame:NSMakeRect(x, y, 1, height)];
    [separator_ setBoxType:NSBoxSeparator];
    [separator_ setTransparent:YES];
    horizontal_ = false;
    nsview_ = separator_;
}

void SeparatorMac::SetHorizontal(bool horizontal) {
    horizontal_ = horizontal;
    if (separator_) {
        NSRect frame = [separator_ frame];
        if (horizontal) {
            [separator_ setFrame:NSMakeRect(frame.origin.x, frame.origin.y, frame.size.width, 1)];
        } else {
            [separator_ setFrame:NSMakeRect(frame.origin.x, frame.origin.y, 1, frame.size.height)];
        }
    }
}

void SeparatorMac::SetBounds(int x, int y, int width, int height) {
    if (separator_) [separator_ setFrame:NSMakeRect(x, y, width, height)];
}

// ============================================================================
// PanelMac
// ============================================================================

PanelMac::PanelMac() : panel_(nil), content_view_(nil) {}
PanelMac::~PanelMac() {
    if (panel_) { [panel_ close]; [panel_ release]; panel_ = nil; }
}

bool PanelMac::Init(int width, int height) {
    if (panel_) return true;
    panel_ = [[NSPanel alloc] initWithContentRect:NSMakeRect(0, 0, width, height)
                                        styleMask:NSWindowStyleMaskTitled |
                                                 NSWindowStyleMaskClosable |
                                                 NSWindowStyleMaskMiniaturizable
                                          backing:NSBackingStoreBuffered
                                            defer:NO];
    if (!panel_) return false;
    
    NSView* content = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, width, height)];
    [panel_ setContentView:content];
    content_view_ = content;
    
    [panel_ center];
    return true;
}

void PanelMac::SetTitle(const std::wstring& title) {
    title_ = title;
    if (panel_) {
        NSString* s = [NSString stringWithUTF8String:std::string(title.begin(), title.end()).c_str()];
        [panel_ setTitle:s];
    }
}

void PanelMac::Show() {
    if (panel_) [panel_ makeKeyAndOrderFront:nil];
}

void PanelMac::Hide() {
    if (panel_) [panel_ orderOut:nil];
}

void PanelMac::Close() {
    if (panel_) [panel_ close];
}

void PanelMac::SetContentView(NSView* view) {
    content_view_ = view;
    if (panel_ && view) [panel_ setContentView:view];
}

void PanelMac::SetAsUtilityWindow() {
    if (panel_) {
        [panel_ setStyleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                            NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskUtilityWindow];
        [panel_ setLevel:NSFloatingWindowLevel];
    }
}

void PanelMac::SetAsModalWindow(NSWindow* parent) {
    if (panel_ && parent) {
        PanelMac* weakSelf = this;
        [parent beginSheet:panel_ completionHandler:^(NSModalResponse returnCode) {
            if (weakSelf->onDismiss_) weakSelf->onDismiss_();
        }];
    }
}

void PanelMac::RunAsSheet(NSWindow* parent, std::function<void()> onDismiss) {
    onDismiss_ = onDismiss;
    if (panel_ && parent) {
        PanelMac* weakSelf = this;
        [parent beginSheet:panel_ completionHandler:^(NSModalResponse returnCode) {
            if (weakSelf->onDismiss_) weakSelf->onDismiss_();
        }];
    }
}

} // namespace mac
} // namespace aura

#endif // AURA_MAC
