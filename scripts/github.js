/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

export default class GitHub {
    constructor() {
        this.base_url = "https://api.github.com";
        this.headers = new Headers({
            Accept: "application/vnd.github.v3+json",
        });
    }

    async get_latest_release(owner, repo) {
        let request = new Request(
            `${this.base_url}/repos/${owner}/${repo}/releases/latest`,
            {
                method: "GET",
                headers: this.headers,
            }
        );

        let response = await fetch(request);

        if (response.status != 200) {
            throw `Request to ${request.url} failed with status code ${request.status}.`
        }

        return await response.json();
    }
}
