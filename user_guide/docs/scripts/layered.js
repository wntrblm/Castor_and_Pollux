document.addEventListener("DOMContentLoaded", () => {
    for (const fig of document.querySelectorAll("figure[data-layered]")) {
        const images = fig.querySelectorAll("img");
        const img_to_btn = new Map();

        const show_image = (which) => {
            for (const img of images) {
                if (img == which) {
                    img.classList.add("active");
                    img_to_btn.get(img).classList.add("active");
                } else {
                    img.classList.remove("active");
                    img_to_btn.get(img).classList.remove("active");
                }
            }
        };

        const btn_div = document.createElement("div");
        btn_div.classList.add("buttons");

        let counter = 1;

        for (const img of images) {
            const btn = document.createElement("button");
            btn_div.append(btn);

            btn.type = "button";
            btn.innerText = img.title || `${counter}`;
            btn.addEventListener("click", () => {
                show_image(img);
            });

            img_to_btn.set(img, btn);

            counter++;
        }

        show_image(images[0]);

        fig.append(btn_div);
    }
});
