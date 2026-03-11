import QtQuick 2.0

Item {
    id: root
    property bool returnValue: false

    property string first
    property string second
    property string third
    property string fourth
    property string fifth
    property string sixth
    property string seventh
    property string eighth

    function success() {
        var a = "Value is %1";
        for (var ii = 0; ii < 10; ++ii) {
            first = a.arg("string");
            second = a.arg(1);
            third = a.arg(true);
            fourth = a.arg(3.345);
            fifth = a.arg(undefined);
            sixth = a.arg(null);
            seventh = a.arg({"test":5});
            eighth = a.arg({"test":5, "again":6});
        }

        if (first != "Value is string") returnValue = false;
        if (second != "Value is 1") returnValue = false;
        if (third != "Value is true") returnValue = false;
        if (fourth != "Value is 3.345") returnValue = false;
        if (fifth != "Value is undefined") returnValue = false;
        if (sixth != "Value is null") returnValue = false;
        if (seventh != "Value is [Object object]") returnValue = false;
        if (eighth != "Value is [Object object]") returnValue = false;
        returnValue = true;
    }

    function multipleArgs() {
        returnValue = false;

        // Test two arguments
        const result2 = "%1 and %2".arg("Hello", "world");
        if (result2 != "Hello and world") return;

        // Test three arguments with mixed types
        const result3 = "%1 is %2 years old and has %3 cats".arg("Alice", 30, 2);
        if (result3 != "Alice is 30 years old and has 2 cats") return;

        // Test four arguments
        const result1 = "%1, %2, %3, %4".arg(1, 2, 3, 4);
        if (result1 != "1, 2, 3, 4") return;

        // Test with boolean and number (boolean converts to 1/0 like in C++)
        const result4 = "Flag: %1, Count: %2".arg(true, 42);
        if (result4 != "Flag: 1, Count: 42") return;

        // Test protection against nested placeholders (one-pass replacement)
        // This is the key advantage of using QString::arg() multi-arg overloads
        const result5 = "%1 %2".arg("%1f", "Hello");
        if (result5 != "%1f Hello") return;

        returnValue = true;
    }

    function failure() {
        returnValue = true;
        const a = "Value is %1";
        // Calling arg() with no arguments should throw an error
        a.arg();
        returnValue = false;
    }
}
