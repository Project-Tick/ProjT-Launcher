import QtQml 2.0

QtObject {
    Component.onCompleted: {
        var stuff = [1, 2, 3, 4]
        for (let a in stuff)
            console.log(a);
    }
}
