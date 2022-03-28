provider "google" {
  project = local.env["project_id"]
  region  = local.env["region"]
  zone    = local.env["zone"]
}
