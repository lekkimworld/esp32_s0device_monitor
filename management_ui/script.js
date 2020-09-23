window.onload = () => {
    // load version data
    fetch("/version.json").then(res => res.json()).then(data => {
        document.getElementById("VERSION_NUMBER").innerText = data.version;
        document.getElementById("VERSION_LASTCHANGE").innerText = data.change;
    })

    document.querySelectorAll("button").forEach(btn => {
        // get rel
        const rel = btn.getAttribute("rel");
        if (!rel) return;

        // load existing data
        fetch(`/${rel}.json`).then(res => res.json()).then(data => {
            Object.keys(data).forEach(key => {
                const elem = document.getElementById(key);
                elem.innerText = data[key];
            })
        })

        // add click listener
        btn.addEventListener("click", ev => {
            const body = {}
            document.querySelectorAll("input").forEach(i => {
                if (i.getAttribute("s0type") === "number") {
                    body[i.name] = Number.parseInt(i.value);
                } else if (i.type === "checkbox") {
                    body[i.name] = i.checked;
                } else {
                    body[i.name] = i.value;
                }

            })
            fetch(`/${rel}.save`, {
                "method": "post",
                "headers": {
                    "Content-Type": "application/json"
                },
                "body": JSON.stringify(body)
            }).then(res => {
                if (res.status !== 200) throw Error(`Error catch - status: ${res.status}, text: ${res.statusText}`)
                return res.json();
            }).then(data => {
                // success

            }).catch(err => {
                // log the error
                console.log(err)

            }).then(() => {
                let count = 15;

                const overlay = document.createElement("div");
                overlay.className = "overlay overlay_blocker";
                overlay.innerText = count;
                document.body.appendChild(overlay);

                const handle = window.setInterval(() => {
                    count--;
                    if (count <= 0) {
                        window.clearInterval(handle);
                        document.location.href = `http://${document.location.hostname}`;
                    } else {
                        overlay.innerText = count;
                    }
                }, 1000);
            })
        })
    })
}