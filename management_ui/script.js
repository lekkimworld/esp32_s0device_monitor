window.onload = () => {
    // load version data
    fetch("/version.json").then(res => res.json()).then(data => {
        document.getElementById("VERSION_NUMBER").innerText = data.version;
        document.getElementById("VERSION_LASTCHANGE").innerText = data.change;
    })

    document.querySelectorAll("button").forEach(btn => {
        btn.addEventListener("click", ev => {
            const rel = ev.target.getAttribute("rel");
            if (!rel) return;
            const body = {}
            document.querySelectorAll("input").forEach(i => {
                body[i.name] = i.type === "checkbox" ? i.checked : i.value;
            })
            fetch(`./${rel}`, {
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