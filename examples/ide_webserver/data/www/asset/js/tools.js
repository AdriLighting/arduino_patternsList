// region ################################################ JSON FUNCTIONS
function hasJsonStructure(str) {
    if (typeof str !== 'string') return false;
    try {
        const result = JSON.parse(str);
        const type = Object.prototype.toString.call(result);
        return type === '[object Object]' 
            || type === '[object Array]';
    } catch (err) {
        return false;
    }
}
function safeJsonParse(str) {
    try {
        return [null, JSON.parse(str)];
    } catch (err) {
        return [err];
    }
}

function printValues(obj) {
    for(var [k, v] in obj) {
        if(obj[k] instanceof Object) {
            printValues(obj[k] );

        } else {
            document.write(obj[k] + "<br>");

        };
    }
};
// printValues(response) ;
function visit(object) {
    if (isIterable(object)) {
        forEachIn(object, function (accessor, child) {
            visit(child);
        });
    }
    else {
        var value = object;
        console.log(value);
    }
}

function forEachIn(iterable, functionRef) {
    for (var accessor in iterable) {
        functionRef(accessor, iterable[accessor]);
    }
}

function isIterable(element) {
    return isArray(element) || isObject(element);
}

function isArray(element) {
    return element.constructor == Array;
}

function isObject(element) {
    return element.constructor == Object;
}

function isUndefined(element) {
    return element == undefined;
}   
// endregion >>>> JSON FUNCTIONS


function removeOptions(selectElement) {
   var i, L = selectElement.options.length - 1;
   for(i = L; i >= 0; i--) {
      selectElement.remove(i);
   }
}