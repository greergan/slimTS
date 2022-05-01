{
    //console.clear();
    console.warn(console.configuration.warn.remainder.text_color)
    console.configure({"warn":{"remainder":{"text_color": "red"}}});


    console.warn(console.configuration.warn.remainder.text_color)
    console.configure({"warn":{"remainder":{"text_color": "green"}}});

    
    console.warn(console.configuration.warn.remainder.text_color)
    //fetch();
/*             (async ()=> {
        async function computeAnswer() {
            return Promise.resolve(42);
        }
        console.log(await computeAnswer());
    })(); */
}