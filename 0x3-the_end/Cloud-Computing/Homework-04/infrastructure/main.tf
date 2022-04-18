module "vpc" {
    source = "./vpc"
    
    resource_group_name = azurerm_resource_group.terraform.name
    resource_group_location = azurerm_resource_group.terraform.location
}